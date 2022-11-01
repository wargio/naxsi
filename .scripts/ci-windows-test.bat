@echo off
rem SPDX-FileCopyrightText: 2022 Alex <alex@staticlibs.net>
rem SPDX-License-Identifier: LGPL-3.0-only

@echo on

set BAD_SLASH_SCRIPT_DIR=%~dp0
set SCRIPT_DIR=%BAD_SLASH_SCRIPT_DIR:\=/%
set NAXSI_DIR=%SCRIPT_DIR%..

pushd "%NAXSI_DIR%" || exit /b 1
python .scripts/naxsi-windows-test-dist.py %NAXSI_DIR%/../nginx-windows/build/dist || exit /b 1
python .scripts/naxsi-gen-tests.py || exit /b 1
python -m unittest discover unit-tests/python/ -v || exit /b 1
popd || exit /b 1