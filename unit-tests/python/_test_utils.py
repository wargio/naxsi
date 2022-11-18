# -* coding: utf-8
# SPDX-FileCopyrightText: 2022 Alex <alex@staticlibs.net>
# SPDX-License-Identifier: LGPL-3.0-only

import os, re, shutil, socket, subprocess, time
from os import path


class NaxsiTestException(Exception):
  pass

def is_posix():
  return "posix" == os.name

def get_naxsi_dir():
  python_dir = path.dirname(__file__)
  unit_tests_dir = path.dirname(python_dir)
  return path.dirname(unit_tests_dir)

def get_nginx_dir():
  naxsi_dir = get_naxsi_dir()
  return path.join(naxsi_dir, "nginx-tmp")

def get_nginx_pid_path():
  nginx_dir = get_nginx_dir()
  if is_posix():
    return path.join(nginx_dir, "naxsi_ut", "nginx.pid")
  else:
    return path.join(nginx_dir, "logs", "nginx.pid")

def get_naxsi_module_so_path():
  nginx_dir = get_nginx_dir()
  return path.join(nginx_dir, "naxsi_ut", "modules", "ngx_http_naxsi_module.so")

def get_naxsi_rules_path():
  naxsi_dir = get_naxsi_dir()
  return path.join(naxsi_dir, "naxsi_rules", "naxsi_core.rules")

def get_naxsi_blocking_rules_dir():
  naxsi_dir = get_naxsi_dir()
  return path.join(naxsi_dir, "naxsi_rules", "blocking")

def get_naxsi_whitelists_rules_dir():
  naxsi_dir = get_naxsi_dir()
  return path.join(naxsi_dir, "naxsi_rules", "whitelists")

def get_error_log_path():
  nginx_dir = get_nginx_dir()
  if is_posix():
    log_dir = path.join(nginx_dir, "naxsi_ut")
  else:
    log_dir = path.join(nginx_dir, "logs")
  return path.join(log_dir, "error.log")

def format_rules_list(rules_dir):
  names = os.listdir(rules_dir)
  paths = list(map(lambda n: path.join(rules_dir, n).replace("\\", "/"), names))
  includes = list(map(lambda p: "include {};".format(p), paths));
  return "\n".join(includes)

def write_nginx_conf(port, http_config, config):
  nginx_dir = get_nginx_dir().replace("\\", "/")
  main_config = ""
  if is_posix():
    naxsi_module_so = get_naxsi_module_so_path()
    main_config = "load_module {};".format(naxsi_module_so)
  naxsi_rules = get_naxsi_rules_path().replace("\\", "/")
  http_config = http_config.replace("$TEST_NGINX_NAXSI_RULES", naxsi_rules)
  blocking_rules = format_rules_list(get_naxsi_blocking_rules_dir())
  http_config = http_config.replace("include $TEST_NGINX_NAXSI_BLOCKING_RULES/*;", blocking_rules)
  config = config.replace("$TEST_NGINX_SERVROOT", nginx_dir)
  whitelists_rules = format_rules_list(get_naxsi_whitelists_rules_dir())
  config = config.replace("include $TEST_NGINX_NAXSI_WHITELISTS_RULES/*;", whitelists_rules)
  conf = """
worker_processes  1;
%s
events {
    worker_connections 1024;
}
http {
%s
    server {
        listen %s;
        server_name localhost;
%s
    }
}
""" % (main_config, http_config, port, config)
  conf_dir = "naxsi_ut" if is_posix() else "conf"
  nginx_conf = path.join(nginx_dir, conf_dir, "nginx.conf")
  with open(nginx_conf, "w", encoding="utf-8") as file:
    file.write(conf)

def start_nginx():
  nginx_dir = get_nginx_dir()
  if is_posix():
    nginx_exe = path.join(nginx_dir, "sbin", "nginx")
  else:
    nginx_exe = path.join(nginx_dir, "nginx.exe")
  if not path.isfile(nginx_exe):
    raise NaxsiTestException("Nginx executable not found, directory: [{}]".format(nginx_dir))
  return subprocess.Popen([nginx_exe], cwd=nginx_dir)

def get_children_pids(parent_pid):
  res = []
  children_pids_text = subprocess.run(["/usr/bin/ps", "-o", "pid", "--ppid", str(parent_pid)],
      stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.decode("utf-8")
  for child_pid_text in children_pids_text.split("\n")[1:-1]:
    child_pid = int(child_pid_text.strip())
    res.append(child_pid)
    #grandchildren_pids = get_children_pids(child_pid)
    #res.extend(grandchildren_pids)
  return res

def read_pid_from_file(nginx_pid_path):
  if not path.isfile(nginx_pid_path):
    return None
  with open(nginx_pid_path, encoding="utf-8") as file:
    return int(file.read().strip())

def kill_nginx(proc=None):
  nginx_pid_path = get_nginx_pid_path()
  if is_posix() or proc is None:
    master_pid = read_pid_from_file(nginx_pid_path)
  else:
    master_pid = proc.pid
  if master_pid is None:
    return
  try:
    if is_posix():
      worker_pid_list = get_children_pids(master_pid)
      pid_list = [master_pid]
      pid_list.extend(worker_pid_list)
      for pid in pid_list:
        subprocess.run(["/usr/bin/kill", "-9", str(pid)],
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
      for pid in pid_list:
        subprocess.run(["/usr/bin/tail", "--pid={}".format(pid), "-f", "/dev/null"],
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    else:
      subprocess.run(["c:/windows/system32/taskkill.exe", "/f", "/t", "/pid", str(proc.pid)],
          stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if proc is not None:
      proc.wait()
  finally:
    if path.isfile(nginx_pid_path):
      os.remove(nginx_pid_path)
    
def read_headers(sock, buf):
  view = memoryview(buf)
  write_pos = 0
  for _ in range(32):
    read = sock.recv_into(view[write_pos:])
    write_pos += read
    for i in range(3, write_pos):
      if b"\r\n\r\n" == buf[i - 3:i + 1]:
        return i + 1, write_pos
    time.sleep(0.1)
  raise NaxsiTestException("Headers read failed, read: [{}]".format(buf[:write_pos]))

def parse_headers(buf, data_begin_pos):
  lines = buf[:data_begin_pos].split(b"\r\n")
  status = int(lines[0].split(b" ")[1])
  headers = {}
  for ln in lines[1:-2]:
    parts = ln.decode("utf-8").split(": ")
    headers[parts[0]] = parts[1]
  return status, headers
    
def read_response(sock):
  buf = bytearray(4096)
  data_begin_pos, write_pos = read_headers(sock, buf)
  status, headers = parse_headers(buf, data_begin_pos)
  clen = int(headers["Content-Length"])
  buffered = write_pos - data_begin_pos
  remaining = clen - buffered
  if remaining > 0:
    view = memoryview(buf)
    for _ in range(32):
      read = sock.recv_into(view[write_pos:])
      write_pos += read
      remaining -= read
      if remaining <= 0:
        break
      time.sleep(0.1)
    if remaining > 0:
      raise NaxsiTestException("Body read failed, read: [{}]".format(buf[:write_pos]))
  body = bytes(buf[data_begin_pos:data_begin_pos + clen])
  return status, headers, body

def includes_header(headers, name):
  lname = name.lower()
  filtered = list(filter(lambda n: n.lower() == lname, headers.keys()))
  return len(filtered) > 0

def send_request(port, url_path, method, headers, data):
  header_lines = [
    "{} {} HTTP/1.1".format(method, url_path),
  ]
  if not includes_header(headers, "Host"):
    header_lines.append("Host: 127.0.0.1:{}".format(port))
  if not includes_header(headers, "User-Agent"):
    header_lines.append("User-Agent: Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10")
  for name, val in headers.items():
    header_lines.append("{}: {}".format(name, val))
  if data is not None and len(data) > 0 and not includes_header(headers, "Content-Length"):
    header_lines.append("Content-Length: {}".format(len(data)))
  header_lines.extend(["", ""])
  req_bytes = "\r\n".join(header_lines).encode("utf-8")
  if data is not None:
    req_bytes += data
  return send_raw_request(port, req_bytes)

def send_raw_request(port, data):
  for _ in range(4):
    try:
      with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
          sock.connect(("127.0.0.1", port))
          sock.sendall(data)
          status, headers, body = read_response(sock)
          return status, headers, body
    except (ConnectionRefusedError, ConnectionResetError):
      time.sleep(0.1)
  raise NaxsiTestException("Connection failed, port: [{}]".format(port))

def write_user_files(files):
  nginx_dir = get_nginx_dir()
  for name, value in files.items():
    filepath = path.join(nginx_dir, "html", name)
    if name != path.basename(name):
      os.makedirs(path.dirname(filepath))
    with open(filepath, "w", encoding="utf-8") as file:
      file.write(value)

def delete_user_files(files):
  nginx_dir = get_nginx_dir()
  for name in files.keys():
    filepath = path.join(nginx_dir, "html", name)
    if name == path.basename(name):
      os.remove(filepath)
    else:
      shutil.rmtree(path.dirname(filepath))

def delete_error_log():
  error_log = get_error_log_path()
  if path.exists(error_log):
    os.remove(error_log)

def error_log_matches_re(regexes):
  error_log = get_error_log_path()
  with open(error_log, encoding="utf-8") as file:
    lines = file.readlines()
    filtered = list(filter(lambda l: " NAXSI_" in l, lines))
    for rg in regexes:
      for fl in filtered:
        if rg.match(fl) is not None:
          return True
  return False


class nginx_runner():
  def __init__(self,
      http_config="",
      config="",
      port=8080,
      user_files={},
    ):
    self.port = port
    self.http_config = http_config
    self.config = config
    self.user_files = user_files

    self.proc = None

  def __enter__(self):
    kill_nginx(self.proc)
    write_nginx_conf(self.port, self.http_config, self.config)
    write_user_files(self.user_files)
    delete_error_log()
    self.proc = start_nginx()
    return self

  def __exit__(self, exc_type, exc_value, traceback):
    if self.proc is not None:
      kill_nginx(self.proc)
    delete_user_files(self.user_files)

  def request(self, url, method="GET", headers={}, data=None, resp_body_required=False):
    if data is not None and type(data) == str:
      data = data.encode("utf-8")
    status, _, body = send_request(self.port, url, method, headers, data)
    if resp_body_required:
      return status, body
    else:
      return status

  def raw_request(self, data):
    if data is not None and type(data) == str:
      data = data.encode("utf-8")
    if data.startswith(b"\n"):
      data = data[1:]
    data = data.replace(b"\\r", b"\r")
    status, _, _ = send_raw_request(self.port, data)
    return status

  def error_log_matches(self, lst):
    regexes = list(map(lambda st: re.compile("^.*{}.*$".format(st)), lst))
    return error_log_matches_re(regexes)
