#!/bin/sh
# SPDX-FileCopyrightText: 2023 wargio <deroad@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only

set -e

WAFEFFICACY_VERSION="v1.0"
NUCLEI_VERSION="2.9.6"

case "$1" in
  "core" | "complete")
    ;;
  *)
    if [ -z "$1" ]; then
        echo "usage: $0 [mode]"
    else
        echo "error: invalid mode option '$1'."
    fi
    echo "supported modes:"
    echo " - core       test efficacy only against naxsi_core.rules"
    echo " - complete   test efficacy against all rules"
    exit 1
    ;;
esac

export NAXSI_CFG_PATH=$(realpath naxsi_rules)
export NAXSI_TMP_PATH=$(realpath nginx-tmp)
export NAXSI_DST_PATH=$(realpath distros/nginx)
export NAXSI_TST_PATH=$(realpath unit-tests/)
export NAXSI_VERSION=$(grep -R "NAXSI_VERSION" naxsi_src/ | cut -d '"' -f2)
export NAXSI_MODE="$1"

check_for() {
    if [ "$1" == "f" ] && [ ! -f "$2" ]; then
        echo "file '$1' does not exist!"
        exit 1
    elif [ "$1" == "d" ] && [ ! -d "$2" ]; then
        echo "directory '$1' does not exist!"
        exit 1
    fi
}

if [ -z "$NAXSI_CFG_PATH" ] || [ -z "$NAXSI_TMP_PATH" ] || [ -z "$NAXSI_DST_PATH" ] ; then
    echo "did you run first ci-build.sh ?"
    exit 1
fi

if [ ! -d "$NAXSI_TMP_PATH/wafefficacy" ]; then
    mkdir -p "$NAXSI_TMP_PATH/wafefficacy-tmp"
    OLDPATH="$PWD"
    cd "$NAXSI_TMP_PATH/wafefficacy-tmp"
    # install wafefficacy
    wget -O wafefficacy.zip "https://github.com/wargio/wafefficacy/releases/download/$WAFEFFICACY_VERSION/wafefficacy-linux-amd64.zip"
    unzip wafefficacy.zip
    mv -v wafefficacy-release "$NAXSI_TMP_PATH/wafefficacy"
    cd "$OLDPATH"
    rm -rf "$NAXSI_TMP_PATH/wafefficacy-tmp"
fi

if [ ! -f "$NAXSI_TMP_PATH/sbin/nuclei" ]; then
    mkdir -p "$NAXSI_TMP_PATH/nuclei-tmp"
    OLDPATH="$PWD"
    cd "$NAXSI_TMP_PATH/nuclei-tmp"
    # install nuclei
    wget -O nuclei.zip "https://github.com/projectdiscovery/nuclei/releases/download/v$NUCLEI_VERSION/nuclei_""$NUCLEI_VERSION""_linux_amd64.zip"
    unzip nuclei.zip || sleep 0
    mv -v nuclei "$NAXSI_TMP_PATH/sbin/"
    cd "$OLDPATH"
    rm -rf "$NAXSI_TMP_PATH/nuclei-tmp"
fi

echo "############################"
echo "    running wafefficacy"
echo "############################"

if [ ! -d "$NAXSI_TMP_PATH/naxsi_ut/logs" ]; then
    mkdir -p "$NAXSI_TMP_PATH/naxsi_ut/logs"
fi

WAFEFFICACY_NGINX_CONF=$(realpath "$NAXSI_TST_PATH/nginx-ci-wafefficacy.conf")
RULES_NGINX_NAXSI_BLOCKING=$(realpath "$NAXSI_CFG_PATH/blocking")
RULES_NGINX_NAXSI_CORE=$(realpath "$NAXSI_CFG_PATH/naxsi_core.rules")
RULES_NGINX_NAXSI_WHITELISTS=$(realpath "$NAXSI_CFG_PATH/whitelists")
TEST_NGINX_NAXSI_MODULE_SO=$(realpath "$NAXSI_TMP_PATH/naxsi_ut/modules/ngx_http_naxsi_module.so")

check_for "f" "$WAFEFFICACY_NGINX_CONF"
check_for "f" "$TEST_NGINX_NAXSI_MODULE_SO"
check_for "f" "$RULES_NGINX_NAXSI_CORE"
check_for "d" "$RULES_NGINX_NAXSI_WHITELISTS"
check_for "d" "$RULES_NGINX_NAXSI_BLOCKING"

cp -v "$WAFEFFICACY_NGINX_CONF" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
sed -i "s#@RULES_NGINX_NAXSI_CORE@#include $RULES_NGINX_NAXSI_CORE;#g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
sed -i "s#@TEST_NGINX_NAXSI_MODULE_SO@#$TEST_NGINX_NAXSI_MODULE_SO#g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"

case "$1" in
  "core")
    sed -i "s#@RULES_NGINX_NAXSI_BLOCKING@##g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
    sed -i "s#@RULES_NGINX_NAXSI_WHITELISTS@##g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
    ;;
  "complete")
    sed -i "s#@RULES_NGINX_NAXSI_BLOCKING@#include $RULES_NGINX_NAXSI_BLOCKING/*;#g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
    sed -i "s#@RULES_NGINX_NAXSI_WHITELISTS@#include $RULES_NGINX_NAXSI_WHITELISTS/*;#g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
    ;;
  *)
    echo "error: invalid mode option '$1'."
    exit 1
    ;;
esac

cd "$NAXSI_TMP_PATH/wafefficacy"

"$NAXSI_TMP_PATH/sbin/nginx" -p "$NAXSI_TMP_PATH/naxsi_ut/" &
PATH="$PATH:$NAXSI_TMP_PATH/sbin" ./run.sh -w "naxsi-$NAXSI_VERSION" -t localhost:4242

jobs -p | xargs kill