# -* coding: utf-8
# SPDX-FileCopyrightText: 2022 Alex <alex@staticlibs.net>
# SPDX-License-Identifier: LGPL-3.0-only

import os, shutil, sys
from os import path

if __name__ == "__main__":
  if 2 != len(sys.argv):
    print("ERROR: The path to Nginx dist must be specified as a first and only argument")
    sys.exit(1)
  src = sys.argv[1]
  if not path.isdir(src):
    print("ERROR: Nginx dist not found on the specified path: [{}]".format(src))
    sys.exit(1)
  scripts_dir = path.dirname(__file__)
  naxsi_dir = path.dirname(scripts_dir)
  nginx_dir = path.join(naxsi_dir, "nginx-tmp")
  if path.exists(nginx_dir):
    shutil.rmtree(nginx_dir)
  os.mkdir(nginx_dir)
  shutil.copytree(path.join(src, "conf"), path.join(nginx_dir, "conf"))
  os.remove(path.join(nginx_dir, "conf", "nginx.conf"))
  shutil.copytree(path.join(src, "logs"), path.join(nginx_dir, "logs"))
  shutil.copytree(path.join(src, "temp"), path.join(nginx_dir, "temp"))
  os.mkdir(path.join(nginx_dir, "html"))
  with open(path.join(nginx_dir, "html", "index.html"), "w", encoding="utf-8") as file:
    file.write("<html>Hello Nginx!<html>")
  shutil.copy(path.join(src, "nginx.exe"), nginx_dir)
  print("Test Nginx dist prepared successfully");