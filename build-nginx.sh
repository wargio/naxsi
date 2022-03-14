#!/bin/sh
set -e

if [ ! -d "$1" ]; then
	echo "$0 <nginx src> [optional: debug|asan]"
	exit 1
fi

NGINX_SRC=$(realpath "$1")
OPTION="$2"
NCPUS=$(getconf _NPROCESSORS_ONLN)

MESON_OPTIONS="--buildtype=release"
if [ "$OPTION" == "debug" ]; then
	MESON_OPTIONS="--buildtype=debugoptimized"
elif [ "$OPTION" == "asan" ]; then
	MESON_OPTIONS="--buildtype=debugoptimized -Db_sanitize=address,undefined"
fi

ROOT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# build librewaf
cd "$ROOT_DIR/libnaxsi"
if [ -d "build" ]; then
	rm -rf build
fi
meson -Ddefault_library=static $MESON_OPTIONS build
ninja -C build

# build nginx module
cd "$NGINX_SRC"
./configure --add-dynamic-module="$ROOT_DIR/naxsi_nginx_module/"

if [ ! -f "$NGINX_SRC/objs/nginx" ]; then
	make -j$NCPUS
fi
make -j$NCPUS modules
