#!/bin/sh
set -e

if [ ! -d "$1" ]; then
	echo "$0 <nginx src> [optional: test_name]"
	exit 1
fi

NGINX_SRC=$(realpath "$1")
TEST_FILE="$2"
ROOT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TEST_DIR="naxsi_test_dir"

killall -9 nginx 2> /dev/null || sleep 0

if [ -d "$ROOT_DIR/$TEST_DIR" ]; then
	rm -rf "$ROOT_DIR/$TEST_DIR"
fi

mkdir "$ROOT_DIR/$TEST_DIR"
cp "$ROOT_DIR/naxsi_config/naxsi_core.rules" "$ROOT_DIR/$TEST_DIR/naxsi_core.rules"

cd "$ROOT_DIR/$TEST_DIR"
export PATH="$NGINX_SRC/objs/:$PATH"
export TEST_NGINX_SERVROOT="$ROOT_DIR/$TEST_DIR/root"
export TEST_NGINX_BINARY="$NGINX_SRC/objs/nginx"
export TEST_NGINX_NAXSI_MODULE_SO="$NGINX_SRC/objs/ngx_http_naxsi_module.so"
export TEST_NGINX_NAXSI_RULES="$ROOT_DIR/naxsi_config/naxsi_core.rules"
if [ -z "$TEST_FILE" ]; then
	prove -r "$ROOT_DIR/naxsi_nginx_module/tests/"*.t
else
	prove -r "$ROOT_DIR/naxsi_nginx_module/tests/$TEST_FILE"
fi

#rm -rf "$ROOT_DIR/$TEST_DIR"