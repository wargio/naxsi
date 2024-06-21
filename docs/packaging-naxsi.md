# **Packaging Naxsi**

This section describes how to build naxsi from source and package it for various distros.

## Packaging for Ubuntu and Debian

> ℹ️ Info
>
> Ubuntu Mantic uses **`libpcre2-dev`** instad of `libpcre3-dev`.

> ℹ️ Info
>
> Debian bookworm uses **`libpcre2-dev`** and requires also **`libperl-dev`**

```bash
# first fetch all required dependencies
sudo apt-get -qqy --no-install-recommends install \
            build-essential ca-certificates dpkg-dev \
            gzip git libgd-dev libgeoip-dev libpcre3-dev \
            libssl-dev libxslt1-dev nginx tar wget zlib1g-dev

# second fetch the nginx sources via apt
apt-get source nginx

# clone naxsi repo with all submodules
git clone --recurse-submodules https://github.com/wargio/naxsi.git

# fetch some missing tools; ps: if you want to help to have a proper deb build, feel free to submit a PR.
git clone --depth=1 https://github.com/wargio/deb-creator.git

# setup nginx build options and deb values.
mkdir -p deb_pkg/
DEB_PKG=$(realpath deb_pkg)
NAXSI_VERSION=$(grep "NAXSI_VERSION" naxsi/naxsi_src/naxsi_const.h | cut -d ' ' -f3 | sed 's/"//g')
LIBPCRE_PACKAGE="libpcre3"
LIBPCRE_VERSION=$(dpkg -s $LIBPCRE_PACKAGE | grep '^Version:' | cut -d ' ' -f2 | cut -d '-' -f1)
NGINX_VERSION=$(dpkg -s nginx | grep '^Version:' | cut -d ' ' -f2 | cut -d '-' -f1)
NGINX_BUILD_OPTS=$(nginx -V 2>&1 | grep "configure arguments:" | cut -d ":" -f2- | sed -e "s#/build/nginx-[A-Za-z0-9]*/#./#g" | sed 's/--add-dynamic-module=[A-Za-z0-9\/\._-]*//g')
echo "LIBPCRE_PACKAGE:  $LIBPCRE_PACKAGE"
echo "LIBPCRE_VERSION:  $LIBPCRE_VERSION"
echo "NGINX_VERSION:    $NGINX_VERSION"
echo "NGINX_BUILD_OPTS: $NGINX_BUILD_OPTS"
echo "Press CTRL+C if you see something wrong with the version or build options"
sleep 5

# build module
cd nginx-$NGINX_VERSION
CMDLINE=$(echo ./configure $NGINX_BUILD_OPTS --add-dynamic-module=../naxsi/naxsi_src/)
eval $CMDLINE
make modules
cd ..

# install files in the temporary folder
mkdir -p "$DEB_PKG/data/usr/lib/nginx/modules/"
mkdir -p "$DEB_PKG/data/usr/share/nginx/modules-available/"
mkdir -p "$DEB_PKG/data/usr/share/naxsi/whitelists"
mkdir -p "$DEB_PKG/data/usr/share/naxsi/blocking"
install -Dm755 naxsi/distros/deb/postinstall.script "$DEB_PKG/postinstall.script"
install -Dm755 naxsi/distros/deb/postremove.script "$DEB_PKG/postremove.script"
install -Dm755 naxsi/distros/deb/preremove.script "$DEB_PKG/preremove.script"
install -Dm644 naxsi/distros/deb/control.install "$DEB_PKG/control.install"
install -Dm755 "nginx-$NGINX_VERSION/objs/ngx_http_naxsi_module.so" "$DEB_PKG/data/usr/lib/nginx/modules/ngx_http_naxsi_module.so"
install -Dm644 naxsi/distros/deb/mod-http-naxsi.conf "$DEB_PKG/data/usr/share/nginx/modules-available/mod-http-naxsi.conf"
install -Dm644 naxsi/distros/nginx/naxsi_block_mode.conf "$DEB_PKG/data/usr/share/naxsi/naxsi_block_mode.conf"
install -Dm644 naxsi/distros/nginx/naxsi_denied_url.conf "$DEB_PKG/data/usr/share/naxsi/naxsi_denied_url.conf"
install -Dm644 naxsi/distros/nginx/naxsi_learning_mode.conf "$DEB_PKG/data/usr/share/naxsi/naxsi_learning_mode.conf"
install -Dm644 naxsi/naxsi_rules/naxsi_core.rules "$DEB_PKG/data/usr/share/naxsi/naxsi_core.rules"
install -Dm644 naxsi/naxsi_rules/whitelists/*.rules "$DEB_PKG/data/usr/share/naxsi/whitelists"
install -Dm644 naxsi/naxsi_rules/blocking/*.rules "$DEB_PKG/data/usr/share/naxsi/blocking"

# add deb details and info.
sed -i "s/@NGINX_VERSION@/$NGINX_VERSION/" "$DEB_PKG/control.install"
sed -i "s/@LIBPCRE_PACKAGE@/$LIBPCRE_PACKAGE/" "$DEB_PKG/control.install"
sed -i "s/@LIBPCRE_VERSION@/$LIBPCRE_VERSION/" "$DEB_PKG/control.install"
sed -i "s/@NAXSI_VERSION@/$NAXSI_VERSION/" "$DEB_PKG/control.install"

# build deb file
./deb-creator/deb-creator "$DEB_PKG"
```

## Packaging for Arch Linux.

```bash
# fetch the needed dependencies
pacman -Syy --needed --noconfirm sudo base-devel git

# clone the repo with all submodules
git clone --recurse-submodules https://github.com/wargio/naxsi.git

# build the package
cd naxsi/distros/arch
makepkg -s
```

# Go Back

[Table of Contents](index.md).
