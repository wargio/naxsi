#!/bin/sh
set -e

NAXSI_ROOT="$PWD"
BUILD_DIR="$NAXSI_ROOT/build"
ALPINE_VERSION=$(cat /etc/alpine-release)
VERSION="unknown"

case "v$ALPINE_VERSION" in
  "v3.13.0" | "v3.13.1" | "v3.13.2" | "v3.13.3" | "v3.13.4" | "v3.13.5")
    VERSION="1.18.0-r13"
    ;;
  "v3.13.6" | "v3.13.7" | "v3.13.8" | "v3.13.9" | "v3.13.10" | "v3.13.11" | "v3.13.12")
    VERSION="1.18.0-r15"
    ;;
  "v3.14.0" | "v3.14.1" | "v3.14.2" | "v3.14.3")
    VERSION="1.20.1-r3"
    ;;
  "v3.14.4" | "v3.14.5" | "v3.14.6" | "v3.15.0" | "v3.15.1" | "v3.15.2" | "v3.15.3" | "v3.15.4")
    VERSION="1.20.2-r0"
    ;;
  "v3.14.7" | "v3.14.8" | "v3.14.9" | "v3.14.10" | "v3.15.5" | "v3.15.6" | "v3.15.7" | "v3.15.8" | "v3.15.9" | "v3.16.0")
    VERSION="1.20.2-r1"
    ;;
  "v3.16.1" | "v3.16.2")
    VERSION="1.22.0-r1"
    ;;
  "v3.16.3" | "v3.16.4" | "v3.16.5" | "v3.16.6" | "v3.17.0" | "v3.17.1" | "v3.17.2" | "v3.17.3" | "v3.17.4")
    VERSION="1.22.1-r0"
    ;;
  "v3.18.0")
    VERSION="1.24.0-r3"
    ;;
  "v3.18.1" | "v3.18.2")
    VERSION="1.24.0-r6"
    ;;
  *)
	echo "error: unsupported version, please open an issue on github."
    exit 1
    ;;
esac

echo "Alpine $ALPINE_VERSION, using $VERSION APKBUILD..."

if [ -d "$BUILD_DIR" ]; then
  rm -rf "$BUILD_DIR"
fi
mkdir "$BUILD_DIR"

echo "$BUILD_DIR/naxsi.key" | abuild-keygen -a
cp -v "$BUILD_DIR/naxsi.key.pub" /etc/apk/keys

cd "$NAXSI_ROOT/distros/alpine/$VERSION"

SRCDEST="$BUILD_DIR" abuild -F fetch
SRCDEST="$BUILD_DIR" abuild -F -c -r -P "$BUILD_DIR"

find "$BUILD_DIR" -name "*.apk" | grep naxsi | xargs -I % mv -v % "$BUILD_DIR"
