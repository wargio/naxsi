# -* coding: utf-8
# SPDX-FileCopyrightText: 2022 Alex <alex@staticlibs.net>
# SPDX-License-Identifier: LGPL-3.0-only

import argparse, os, shutil, socket, subprocess, sys, time
from collections import namedtuple
from os import path
from traceback import format_exc
from urllib import request 
from urllib.error import URLError

NginxTest = namedtuple("NginxTest", "filename name user_files main_config http_config config more_headers request raw_request error_code")

def validate_args(path_to_nginx_dist, test_file_or_dir):
  nginx_exe = path.join(path_to_nginx_dist, "nginx.exe")
  if not path.isdir(path_to_nginx_dist) or not path.isfile(nginx_exe):
    print("ERROR: nginx.exe not found in dir: [{}]".format(path_to_nginx_dist))
    sys.exit(1)
  if not path.exists(test_file_or_dir):
    print("ERROR: tests not found on path: [{}]".format(test_file_or_dir))
    sys.exit(1)

def is_test_file(dirpath, filename):
  if not filename.endswith(".t"):
    return False
  filepath = path.join(dirpath, filename)
  return path.isfile(filepath)

def read_list_of_test_files(test_file_or_dir):
  if path.isfile(test_file_or_dir):
    return [test_file_or_dir]
  elif path.isdir(test_file_or_dir):
    lst = os.listdir(test_file_or_dir)
    filtered = list(filter(lambda f: is_test_file(test_file_or_dir, f), lst))
    filtered.sort()
    return list(map(lambda f: path.join(test_file_or_dir, f), filtered))
  else:
    print("ERROR: tests not found in directory: [{}]".format(test_file_or_dir))
    sys.exit(1)

def collect_section(lines, idx, dest):
  i = idx + 1 
  while not lines[i].startswith("--- "):
    dest.append(lines[i])
    i += 1
  return i

def trim_test_lines(lines):
  last_nonempty_idx = len(lines)
  for i in reversed(range(len(lines))):
    if len(lines[i]) == 0:
      last_nonempty_idx = i
    else:
      break
  return lines[:last_nonempty_idx]

def parse_test(lines, test_file, line_num):
  lines = trim_test_lines(lines)
  if len(lines) == 0:
    return None
  filename = path.basename(test_file)
  name = lines[0][4:].strip()
  user_files = []
  main_config = []
  http_config = []
  config = []
  more_headers = []
  request = []
  raw_request = []
  error_code = 0
  idx = 1
  if lines[idx].lstrip().startswith("--- user_files"):
    idx = collect_section(lines, idx, user_files)
  if lines[idx].lstrip().startswith("--- main_config"):
    idx = collect_section(lines, idx, main_config)
  if lines[idx].lstrip().startswith("--- main_config"):
    idx = collect_section(lines, idx, main_config)
  if lines[idx].lstrip().startswith("--- http_config"):
    idx = collect_section(lines, idx, http_config)
  if lines[idx].lstrip().startswith("--- user_files"):
    idx = collect_section(lines, idx, user_files)
  if lines[idx].lstrip().startswith("--- config"):
    idx = collect_section(lines, idx, config)
  if lines[idx].lstrip().startswith("--- more_headers"):
    idx = collect_section(lines, idx, more_headers)
  if lines[idx].lstrip().startswith("--- request"):
    idx = collect_section(lines, idx, request)
  if lines[idx].lstrip().startswith("--- raw_request"):
    idx = collect_section(lines, idx, raw_request)
  if not lines[idx].lstrip().startswith("--- error_code: "):
    print("ERROR: Cannot parse test defintion, file: [{}], line: [{}]".format(test_file,  line_num + idx - 2))
    sys.exit(1)
  prefix_len = len("--- error_code: ")
  error_code = int(lines[idx].lstrip()[prefix_len:])
  return NginxTest(filename, name, user_files, main_config, http_config, config, more_headers, request, raw_request, error_code)

def read_list_of_tests(test_file):
  tests = []
  with open(test_file, encoding="utf-8") as file:
    data_reached = False
    test_lines = []
    line_num = 0
    for line in file:
      line_num += 1
      stripped = line.rstrip()
      if stripped.startswith("#"):
        continue
      if not data_reached:
        if "__DATA__" == stripped:
          data_reached = True
        continue
      if stripped.startswith("==="):
        if len(test_lines) > 0:
          test = parse_test(test_lines, test_file, line_num - len(test_lines))
          if test is not None:
            tests.append(test)
        test_lines = []
      test_lines.append(stripped)
    if len(test_lines) > 0:
      test = parse_test(test_lines, test_file, line_num - len(test_lines))
      if test is not None:
        tests.append(test)
  return tests

def collect_tests(test_files_list):
  tests = []
  for test_file in test_files_list:
    tests_read = read_list_of_tests(test_file)
    tests.extend(tests_read)
  return tests
      
def prepare_nginx_dir(path_to_nginx_dist):
  scripts_dir = path.dirname(__file__)
  naxsi_dir = path.dirname(scripts_dir)
  nginx_dir = path.join(naxsi_dir, "nginx-tmp")
  if path.exists(nginx_dir):
    shutil.rmtree(nginx_dir)
  os.mkdir(nginx_dir)
  src = path_to_nginx_dist
  shutil.copytree(path.join(src, "conf"), path.join(nginx_dir, "conf"))
  os.remove(path.join(nginx_dir, "conf", "nginx.conf"))
  shutil.copytree(path.join(src, "logs"), path.join(nginx_dir, "logs"))
  shutil.copytree(path.join(src, "temp"), path.join(nginx_dir, "temp"))
  os.mkdir(path.join(nginx_dir, "html"))
  with open(path.join(nginx_dir, "html", "index.html"), "w", encoding="utf-8") as file:
    file.write("<html>Hello Nginx!<html>")
  shutil.copy(path.join(src, "nginx.exe"), nginx_dir)
  return nginx_dir

def format_test_lines(test, nginx_dir):
  scripts_dir = path.dirname(__file__)
  naxsi_dir = path.dirname(scripts_dir)
  naxsi_rules = path.join(naxsi_dir, "naxsi_rules", "naxsi_core.rules")
  naxsi_rules_slashed = naxsi_rules.replace("\\", "/")
  nginx_dir_slashed = nginx_dir.replace("\\", "/")
  for i in range(len(test.main_config)):
    line = test.main_config[i]
    if line.startswith("load_module"):
      test.main_config[i] = "#" + line
  for i in range(len(test.http_config)):
    line = test.http_config[i]
    line = line.replace("$TEST_NGINX_NAXSI_RULES", naxsi_rules_slashed)
    test.http_config[i] = "    " + line
  for i in range(len(test.config)):
    line = test.config[i]
    line = line.replace("$TEST_NGINX_SERVROOT", nginx_dir_slashed)
    test.config[i] = "        " + line
  request_lines_to_pop = []
  for i in range(len(test.request)):
    line = test.request[i]
    if line.lstrip().startswith("use "):
      request_lines_to_pop.append(i)
  for idx in request_lines_to_pop:
    test.request.pop(idx)
  if len(test.request) > 0 and test.request[0].startswith("\""):
    test.request[0] = test.request[0][1:]
    test.request[-1] = test.request[-1][:-1]
    for i in range(len(test.request)):
      line = test.request[i]
      test.request[i] = line.replace("\\r\\n", "\r\n").replace("\\\"", "\"").replace("\\\\\"", "\\\"")
  if len(test.raw_request) > 0 and test.raw_request[0].startswith("\""):
    test.raw_request[0] = test.raw_request[0][1:]
    test.raw_request[-1] = test.raw_request[-1][:-1]
    for i in range(len(test.raw_request)):
      line = test.raw_request[i]
      test.raw_request[i] = line.replace("\\r", "\r").replace("\\\"", "\"")

def write_nginx_conf(nginx_dir, test):
  conf = """
worker_processes  1;
%s
events {
    worker_connections 1024;
}
http {
%s
    server {
        listen 8080;  
%s
    }
}
""" % ("\n".join(test.main_config), "\n".join(test.http_config), "\n".join(test.config))
  nginx_conf = path.join(nginx_dir, "conf", "nginx.conf")
  with open(nginx_conf, "w", encoding="utf-8") as file:
    file.write(conf)

def start_nginx(nginx_dir):
  nginx_exe = path.join(nginx_dir, "nginx.exe")
  proc = subprocess.Popen([nginx_exe], cwd=nginx_dir)
  return proc.pid

def kill_nginx(nginx_dir, pid):
  if pid > 0:
    with open(path.join(nginx_dir, "logs", "kill_out.txt"), "wb") as file:
      subprocess.run(["taskkill", "/f", "/t", "/pid", str(pid)], stdout=file, stderr=subprocess.STDOUT)
  nginx_pid = path.join(nginx_dir, "logs", "nginx.pid")
  if path.isfile(nginx_pid):
    os.remove(nginx_pid)

class NginxHTTPErrorHandler(request.BaseHandler):
    def http_error_400(self, request, response, code, msg, hdrs):
        return response
    def http_error_404(self, request, response, code, msg, hdrs):
        return response
    def http_error_412(self, request, response, code, msg, hdrs):
        return response

def parse_req_method(req):
  space_idx = req[0].find(" ")
  return req[0][:space_idx]

def parse_req_data(req, method):
  if method not in ["POST", "PATCH"]:
    return None
  rn_idx = req[0].find("\r\n")
  inline = ""
  if rn_idx > 0:
    inline = req[0][rn_idx:]
  res = inline
  if len(req) > 1:
    res += "\n".join(req[1:])
  if res.startswith("\r\n"):
    res = res[2:]
  return res.encode("utf-8")

def parse_req_url(req):
  space_idx = req[0].find(" ")
  rn_idx = req[0].find("\r\n")
  if rn_idx > 0:
    url_path = req[0][space_idx + 1:rn_idx]
  else:
    url_path = req[0][space_idx + 1:]
  return "http://127.0.0.1:8080{}".format(url_path)

def parse_headers(more_headers):
  headers = {}
  for line in more_headers:
    idx = line.find(": ")
    name = line[:idx]
    value = line[idx + 2:]
    headers[name] = value
  return headers

def send_request(req, more_headers):
  method = parse_req_method(req)
  data = parse_req_data(req, method)
  headers = parse_headers(more_headers)
  url = parse_req_url(req)
  #print(headers)
  #print(data)
  #print(len(data))
  r = request.Request(url, method=method, data=data, headers=headers)
  opener = request.build_opener(request.HTTPHandler, NginxHTTPErrorHandler)
  for _ in range(4):
    try:
      #with opener.open(r, timeout=2) as resp:
      with opener.open(r) as resp:
        resp.read()
        return resp.status
    except (ConnectionResetError, URLError):
      time.sleep(0.1)

def send_raw_request(raw_req):
  data = ("\n".join(raw_req) + "\n").encode("utf-8")
  #print(data)
  for _ in range(4):
    try:
      with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
          sock.connect(("127.0.0.1", 8080))
          sock.sendall(data)
          buf = []
          ch = None
          ch_prev = None
          for i in range(1024):
            ch = sock.recv(1)
            if b"\r" == ch_prev and b"\n" == ch:
              break
            buf.append(ch)
            ch_prev = ch
          resp = (b"".join(buf)).decode("utf-8") 
          if resp.startswith("HTTP/"):
            space_idx = resp.find(" ")
            return int(resp[space_idx + 1:space_idx + 4])
          else:
            return -1
    except ConnectionResetError:
      time.sleep(0.1)

def parse_user_files(user_files):
  res = {}
  name = None
  for line in user_files:
    stripped = line.strip()
    if stripped.startswith(">>> "):
      name = stripped[4:]
      res[name] = ""
    else:
      res[name] += line
  return res
    
def write_user_files(nginx_dir, files):
  for name, value in files.items():
    filepath = path.join(nginx_dir, "html", name)
    if name != path.basename(name):
      os.makedirs(path.dirname(filepath))
    with open(filepath, "w", encoding="utf-8") as file:
      file.write(value)

def delete_user_files(nginx_dir, filenames):
  for name in filenames:
    filepath = path.join(nginx_dir, "html", name)
    if name == path.basename(name):
      os.remove(filepath)
    else:
      shutil.rmtree(path.dirname(filepath))

def run_test(nginx_dir, test):
  print("{}: {}".format(test.filename, test.name))
  if len(test.request) > 0 and test.request[0].startswith("[["):
    print("*SKIPPED*: {}".format("'[[' request eval is not supported"))
    return "SKIPPED"
  format_test_lines(test, nginx_dir)
  write_nginx_conf(nginx_dir, test)
  files_dict = parse_user_files(test.user_files)
  write_user_files(nginx_dir, files_dict)
  pid = -1 
  try:
    pid = start_nginx(nginx_dir)
    if len(test.raw_request) > 0:
      ec = send_raw_request(test.raw_request)
    else:
      ec = send_request(test.request, test.more_headers)
    if ec != test.error_code:
      print("*FAILED*: expected code: [{}], actual code: [{}]".format(test.error_code, ec))
      return "FAILED"
  except:
    print(format_exc())
    return "ERROR"
  finally:
    kill_nginx(nginx_dir, pid)
    delete_user_files(nginx_dir, files_dict.keys())


def run_test_list(nginx_dir, tests):
  success = 0
  skipped = 0
  failed = 0
  error = 0
  for test in tests:
    res = run_test(nginx_dir, test)
    if "SKIPPED" == res:
      skipped += 1
    elif "FAILED" == res:
      failed += 1
    elif "ERROR" == res:
      error += 1
    else:
      success += 1
  print("\nTests run complete:")
  print("All: {}".format(len(tests)))
  print("Successful: {}".format(success))
  print("Skipped: {}".format(skipped))
  print("Failed: {}".format(failed))
  print("Errors: {}".format(error))


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="Runs NAXSI test suite.")
  parser.add_argument("path_to_nginx_dist", help="Path to a nginx-windows dist directory that contains nginx.exe binary")
  parser.add_argument("test_file_or_dir", help="Path to a test file or a directory containing test files")
  args = parser.parse_args()
  validate_args(args.path_to_nginx_dist, args.test_file_or_dir)
  test_files_list = read_list_of_test_files(args.test_file_or_dir)
  tests = collect_tests(test_files_list)
  nginx_dir = prepare_nginx_dir(args.path_to_nginx_dist)
  run_test_list(nginx_dir, tests)
