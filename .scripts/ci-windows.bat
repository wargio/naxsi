@echo off
rem SPDX-FileCopyrightText: 2022 Alex <alex@staticlibs.net>
rem SPDX-License-Identifier: LGPL-3.0-only

@echo on

set BAD_SLASH_SCRIPT_DIR=%~dp0
set SCRIPT_DIR=%BAD_SLASH_SCRIPT_DIR:\=/%
set NAXSI_DIR=%SCRIPT_DIR%..

cd ..
git clone https://github.com/noproxy-http/nginx-windows.git
cd nginx-windows
git submodule update --init
robocopy %NAXSI_DIR%/naxsi_src modules/naxsi_src /e /nfl /ndl /njh /njs /nc /ns /np
scripts\build.bat