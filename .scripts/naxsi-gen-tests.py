# -* coding: utf-8
# SPDX-FileCopyrightText: 2022 Alex <alex@staticlibs.net>
# SPDX-License-Identifier: LGPL-3.0-only

import os, sys
from collections import namedtuple
from os import path, replace
import re

NginxTest = namedtuple("NginxTest", "filename name user_files main_config http_config config more_headers request raw_request error_code, error_log no_error_log response_body")

unique_fun_names = set()

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
  error_log = []
  no_error_log = []
  response_body = []
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
  if idx < len(lines) - 2 and lines[idx + 1].lstrip().startswith("--- error_log"):
    idx += 2
    while idx < len(lines) and (not lines[idx].startswith("--- ") or lines[idx].startswith("=== ")):
      error_log.append(lines[idx])
      idx += 1
    idx -= 1
  if idx < len(lines) - 2 and lines[idx + 1].lstrip().startswith("--- no_error_log"):
    idx += 2
    while idx < len(lines) and (not lines[idx].startswith("--- ") or lines[idx].startswith("=== ")):
      no_error_log.append(lines[idx])
      idx += 1
    idx -= 1
  if idx < len(lines) - 1 and lines[idx + 1].lstrip().startswith("--- response_body"):
    idx+=1
    response_body.append(lines[idx][18:].strip())
  return NginxTest(filename, name, user_files, main_config, http_config, config, more_headers, request, raw_request, error_code, error_log, no_error_log, response_body)

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

def format_array_eval_line(line):
    line = line[2:-2].strip()
    elems = line.split(",")
    for i in range(len(elems)):
      elems[i] = elems[i].strip()
      xidx = elems[i].find("\"x")
      if xidx > 0:
        base = elems[i][1:xidx]
        count = int(elems[i][xidx+2:])
        elems[i] = ""
        for j in range(count):
          elems[i] += base
      else:
        elems[i] = elems[i][1:-1]
    return "".join(elems)

def format_test_lines(test):
  for i in range(len(test.http_config)):
      test.http_config[i] = test.http_config[i].replace("\\", "\\\\")
      test.http_config[i] = "        {}".format(test.http_config[i])
  for i in range(len(test.config)):
      test.config[i] = "        {}".format(test.config[i])
  request_lines_to_pop = []
  for i in range(len(test.request)):
    line = test.request[i]
    if line.lstrip().startswith("use "):
      request_lines_to_pop.append(i)
    if line.startswith("[[") and line.endswith("]]"):
      test.request[i] = format_array_eval_line(line)
  for idx in request_lines_to_pop:
    test.request.pop(idx)
  if len(test.request) > 0 and test.request[0].startswith("\""):
    test.request[0] = test.request[0][1:]
    test.request[-1] = test.request[-1][:-1]
  for i in range(len(test.request)):
    line = test.request[i]
    test.request[i] = line.replace("\\\"", "\"").replace("\x00", "\\x00")
  if len(test.raw_request) > 0 and test.raw_request[0].startswith("\""):
    test.raw_request[0] = test.raw_request[0][1:]
    test.raw_request[-1] = test.raw_request[-1][:-1]
    for i in range(len(test.raw_request)):
      line = test.raw_request[i]
      test.raw_request[i] = line.replace("\\\"", "\"")
  if len(test.error_log) > 0:
    el = test.error_log
    for i in range(len(el)):
      if el[i].startswith("["):
        el[i] = el[i][1:].lstrip()
      if el[i].startswith("qr@"):
        el[i] = el[i][3:].lstrip()
      if el[i].endswith("]"):
        el[i] = el[i][:-1].rstrip()
      if el[i].endswith(","):
        el[i] = el[i][:-1].rstrip()
      if el[i].endswith("@"):
        el[i] = el[i][:-1].rstrip()
      el[i] = el[i].replace("server=localhost", "server=127.0.0.1")
    if len(el[-1]) == 0:
      el.pop()
  if len(test.response_body) > 0:
    for i in range(len(test.response_body)):
      test.response_body[i] = test.response_body[i].replace("localhost", "127.0.0.1")

def hotpatch_test(test):
  if "TEST 24: Testing MULTIPART POSTs" == test.name:
    test.more_headers[1] = test.more_headers[1].replace(
      "Content-Length: 355",
      "Content-Length: 353",
    )

def parse_req_method(req):
  space_idx = req[0].find(" ")
  return req[0][:space_idx]

def parse_req_data(req, method):
  if method not in ["POST", "PATCH"]:
    return None
  rn_idx = req[0].find("\\r\\n")
  inline = ""
  if rn_idx > 0:
    inline = req[0][rn_idx:]
  res = inline
  if len(req) > 1:
    res += "\n".join(req[1:])
  if res.startswith("\\r\\n"):
    res = res[4:]
  if res.endswith("\\r\\n\\r\\n"):
    res = res[:-8]
  return res

def parse_req_url(req):
  space_idx = req[0].find(" ")
  rn_idx = req[0].find("\\r\\n")
  if rn_idx > 0:
    return req[0][space_idx + 1:rn_idx]
  else:
    return req[0][space_idx + 1:]

def parse_headers(more_headers):
  headers = {}
  for line in more_headers:
    idx = line.find(": ")
    name = line[:idx]
    value = line[idx + 2:]
    headers[name] = value
  return headers

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

def gen_file_header(test_file_name_noext):
  cls_name = test_file_name_noext[2:].replace("-", "_")
  return """# -* coding: utf-8
# SPDX-FileCopyrightText: NAXSI project
# SPDX-License-Identifier: LGPL-3.0-only

import unittest
from _test_utils import nginx_runner

class {}(unittest.TestCase):""".format(cls_name)

def gen_function_header(test):
  fun_name = test.name.strip()
  fun_name = re.sub(r'[^a-zA-Z\d_]', '_', fun_name)
  if not fun_name.startswith("test_"):
    fun_name = "test_{}".format(fun_name)
  while fun_name in unique_fun_names:
    fun_name += "_"
  unique_fun_names.add(fun_name)
  docstring = test.name.replace("\\x", "\\\\x")
  return '''
  def {}(self):
    """
    {}
    """'''.format(fun_name, docstring)

def gen_runner_init(test):
  res = '''    with nginx_runner('''
  if len(test.http_config) > 0:
    http_config = "\n".join(test.http_config)
    res += '''
      http_config="""
{}
      """,'''.format(http_config)
  if len(test.config) > 0:
    config = "\n".join(test.config)
    res += '''
      config="""
{}
      """,'''.format(config)
  if len(test.user_files) > 0:
    res += '''
      user_files={'''
    files = parse_user_files(test.user_files)
    for name, value in files.items():
      res += '''
        "{}": "{}",'''.format(name, value)
    res += '''
      },'''
  res += '''
    ) as nr:'''
  return res;

def gen_request(test):
  if len(test.response_body) > 0:
    res = '''      ec, resp_body = nr.request('''
  else:
    res = '''      ec = nr.request('''
  url = parse_req_url(test.request).replace("\"", "\\\"")
  res += '''
        url="{}",'''.format(url)
  method = parse_req_method(test.request)
  if "GET" != method:
    res += '''
        method="{}",'''.format(method)
  if len(test.more_headers) > 0:
    res += '''
        headers={'''
    headers = parse_headers(test.more_headers)
    for name, value in headers.items():
      res += '''
          "{}": "{}",'''.format(name, value)
    res += '''
        },'''
  data = parse_req_data(test.request, method)
  if data is not None:
    res += '''
        data="""{}""",'''.format(data)
  if len(test.response_body) > 0:
    res += '''
        resp_body_required=True'''
  res += '''
      )'''. format(test.error_code)
  return res

def gen_raw_request(test):
  raw_req = "\n".join(test.raw_request)
  return '''      ec = nr.raw_request("""
{}""")'''.format(raw_req)

def gen_checks(test):
  res = "      self.assertEqual(ec, {})".format(test.error_code)
  if len(test.error_log) > 0:
    res += '''
      elm = nr.error_log_matches(['''
    for line in test.error_log:
      res += '''
        "{}",'''.format(line)
    res += '''
      ])
      self.assertTrue(elm)'''
  if len(test.no_error_log) > 0:
    res += '''
      nelm = nr.error_log_matches(['''
    for line in test.no_error_log:
      res += '''
        "{}",'''.format(line)
    res += '''
      ])
      self.assertFalse(nelm)'''
  if len(test.response_body) > 0:
    response_body = "\n".join(test.response_body)
    res += '''
      self.assertEqual(resp_body, """{}""".encode("utf-8"))'''.format(response_body)
  return res

def gen_test_function(test):
  header = gen_function_header(test)
  runner = gen_runner_init(test)
  if len(test.request) > 0:
    send_req = gen_request(test)
  else:
    send_req = gen_raw_request(test)
  checks = gen_checks(test)
  return '''
{}
{}
{}
{}'''.format(header, runner, send_req, checks)

def write_python_test_file(test_file_name, test_list):
  scripts_dir = path.dirname(__file__)
  naxsi_dir = path.dirname(scripts_dir)
  dest_dir = path.join(naxsi_dir, "unit-tests", "python")
  test_file_name_noext = path.splitext(test_file_name)[0]
  test_file_name_nodash = test_file_name_noext.replace("-", "_")
  dest_file_name = "test_{}.py".format(test_file_name_nodash)
  dest_file_path = path.join(dest_dir, dest_file_name)
  if path.exists(dest_file_path):
    os.remove(dest_file_path)
  with open(dest_file_path, "w", encoding="utf-8") as file:
    file.write(gen_file_header(test_file_name_noext))
    for test in test_list:
      hotpatch_test(test)
      format_test_lines(test)
      file.write(gen_test_function(test))
    file.write("\n")
  

if __name__ == "__main__":
  scripts_dir = path.dirname(__file__)
  naxsi_dir = path.dirname(scripts_dir)
  tests_dir = path.join(naxsi_dir, "unit-tests", "tests")
  test_files_list = read_list_of_test_files(tests_dir)
  for test_file_path in test_files_list[:]:
    unique_fun_names.clear()
    test_list = read_list_of_tests(test_file_path)
    test_file_name = path.basename(test_file_path)
    write_python_test_file(test_file_name, test_list[:])