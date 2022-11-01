@echo off
rem SPDX-FileCopyrightText: 2022 Alex <alex@staticlibs.net>
rem SPDX-License-Identifier: LGPL-3.0-only

@echo on

set BAD_SLASH_SCRIPT_DIR=%~dp0
set SCRIPT_DIR=%BAD_SLASH_SCRIPT_DIR:\=/%
set NAXSI_DIR=%SCRIPT_DIR%..
set NW_DIR=%NAXSI_DIR%/../nginx-windows

set NGINX_VERSION=%1

pushd .. || exit /b 1
git clone --branch 1.22.1-1 https://github.com/noproxy-http/nginx-windows.git || exit /b 1
popd || exit /b 1

pushd "%NW_DIR%" || exit /b 1
git submodule update --init || exit /b 1
popd || exit /b 1

pushd "%NW_DIR%/sources/nginx" || exit /b 1
git checkout release-%NGINX_VERSION% || exit /b 1
popd || exit /b 1

robocopy %NAXSI_DIR%/naxsi_src %NW_DIR%/modules/naxsi_src /e /nfl /ndl /njh /njs /nc /ns /np

pushd "%NW_DIR%" || exit /b 1
scripts\build.bat "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvars64.bat" nossl
popd || exit /b 1