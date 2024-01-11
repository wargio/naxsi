#!/bin/sh
# SPDX-FileCopyrightText: 2024 wargio <deroad@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only

set -e

OPENAPPSEC_DATASET="https://downloads.openappsec.io/waf-comparison-project/legitimate.zip"
WAF_CHECK_BRANCH="master"
WORK_DIR=$(pwd)

export NAXSI_CFG_PATH=$(realpath naxsi_rules)
export NAXSI_TMP_PATH=$(realpath nginx-tmp)
export NAXSI_WAF_CHECK=$(realpath waf-check-tmp)
export NAXSI_DST_PATH=$(realpath distros/nginx)
export NAXSI_TST_PATH=$(realpath unit-tests/)
export NAXSI_VERSION=$(grep -R "NAXSI_VERSION" naxsi_src/ | cut -d '"' -f2)

case "$1" in
  "core" | "complete" | "setup")
    ;;
  *)
    if [ -z "$1" ]; then
        echo "usage: $0 [mode]"
    else
        echo "error: invalid mode option '$1'."
    fi
    echo "supported modes:"
    echo " - setup      setup the folders and assets and exit."
    echo " - core       test efficacy only against naxsi_core.rules"
    echo " - complete   test efficacy against all rules"
    exit 1
    ;;
esac

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

if [ ! -d "$NAXSI_WAF_CHECK" ]; then
    git clone --depth 1 --branch "$WAF_CHECK_BRANCH" "https://github.com/crowdsecurity/hub.git" "$NAXSI_WAF_CHECK"
fi

if [ ! -f "$NAXSI_WAF_CHECK/waf-check/waf-check" ]; then
    cd "$NAXSI_WAF_CHECK/waf-check/"
    go build -v || exit 1
    cd "$WORK_DIR"
fi

if [ ! -d "$NAXSI_TMP_PATH/output" ]; then
    mkdir "$NAXSI_TMP_PATH/output"
fi

if [ ! -d "$NAXSI_WAF_CHECK/dataset" ]; then
    if [ ! -f "$NAXSI_WAF_CHECK/dataset.zip" ]; then
        echo "downloading $OPENAPPSEC_DATASET"
        curl -o "$NAXSI_WAF_CHECK/dataset.zip" "$OPENAPPSEC_DATASET"
    fi
    mkdir "$NAXSI_WAF_CHECK/dataset"
    unzip "$NAXSI_WAF_CHECK/dataset.zip" -d "$NAXSI_WAF_CHECK/dataset"
    rm "$NAXSI_WAF_CHECK/dataset.zip"
fi

if [ -d "$NAXSI_TMP_PATH/naxsi_ut/logs" ]; then
    rm -rf "$NAXSI_TMP_PATH/naxsi_ut/logs/"
fi

mkdir -p "$NAXSI_TMP_PATH/naxsi_ut/logs"

WAF_CHECK_NGINX_CONF=$(realpath "$NAXSI_TST_PATH/nginx-ci-waf-check.conf")
WAF_CHECK_YAML=$(realpath "$NAXSI_TST_PATH/waf-check.yaml")
RULES_NGINX_NAXSI_BLOCKING=$(realpath "$NAXSI_CFG_PATH/blocking")
RULES_NGINX_NAXSI_CORE=$(realpath "$NAXSI_CFG_PATH/naxsi_core.rules")
RULES_NGINX_NAXSI_WHITELISTS=$(realpath "$NAXSI_CFG_PATH/whitelists")
TEST_NGINX_LOG_DIR=$(realpath "$NAXSI_TMP_PATH/naxsi_ut/logs")
TEST_NGINX_NAXSI_MODULE_SO=$(realpath "$NAXSI_TMP_PATH/naxsi_ut/modules/ngx_http_naxsi_module.so")

check_for "f" "$WAF_CHECK_NGINX_CONF"
check_for "f" "$TEST_NGINX_NAXSI_MODULE_SO"
check_for "f" "$RULES_NGINX_NAXSI_CORE"
check_for "d" "$RULES_NGINX_NAXSI_WHITELISTS"
check_for "d" "$RULES_NGINX_NAXSI_BLOCKING"

cp -v "$WAF_CHECK_NGINX_CONF" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
sed -i "s#@RULES_NGINX_NAXSI_CORE@#include $RULES_NGINX_NAXSI_CORE;#g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
sed -i "s#@TEST_NGINX_NAXSI_MODULE_SO@#$TEST_NGINX_NAXSI_MODULE_SO#g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"
sed -i "s#@TEST_NGINX_LOG_DIR@#$TEST_NGINX_LOG_DIR#g" "$NAXSI_TMP_PATH/naxsi_ut/nginx.conf"



case "$1" in
  "setup")
    exit 0
    ;;
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

echo "############################"
echo "    running waf-test"
echo "############################"

function stop_nginx {
  echo "Stopping nginx"
  jobs -p | xargs kill
}

"$NAXSI_TMP_PATH/sbin/nginx" -p "$NAXSI_TMP_PATH/naxsi_ut/" &
trap stop_nginx EXIT

cd "$NAXSI_WAF_CHECK/waf-check/"

"$NAXSI_WAF_CHECK/waf-check/waf-check" -status-code 406 -dataset "$NAXSI_WAF_CHECK/dataset" -output "$NAXSI_TMP_PATH/output" -config "$WAF_CHECK_YAML"

