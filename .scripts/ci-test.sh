#!/bin/sh
# SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only

RUN_TEST="$1"

export NAXSI_CFG_PATH=$(realpath naxsi_rules/)
export NAXSI_TMP_PATH=$(realpath nginx-tmp/)
export NAXSI_TST_PATH=$(realpath unit-tests/)

if [ -z "$NAXSI_CFG_PATH" ] || [ -z "$NAXSI_TMP_PATH" ] || [ -z "$NAXSI_TST_PATH" ] ; then
    echo "did you run first ci-build.sh ?"
    exit 1
fi

echo "############################"
echo "      running tests"
echo "############################"

cp -v "$NAXSI_TST_PATH/nginx-ci.conf" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
openssl req -batch -x509 -nodes -days 365 -newkey rsa:2048 -keyout "$NAXSI_TMP_PATH/nginx.key" -out "$NAXSI_TMP_PATH/nginx.crt"

python .scripts/naxsi-gen-tests.py
python -m unittest discover ./unit-tests/python/ -v
