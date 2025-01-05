#!/bin/sh
# SPDX-FileCopyrightText: 2025 deroad <deroad@kumo.xn--q9jyb4c>
# SPDX-License-Identifier: LGPL-3.0-only

patch_apt_source() {
    # enable deb-src
    sed -i 's/^# deb-src/deb-src/g' "$1"
    if [ ! -z "$(cat "$1" | grep '^Types: deb$')" ]; then
        echo "patching deb822 style $1"
        sed -i 's/^Types: deb$/Types: deb deb-src/g' "$1"
    elif [ -z "$(cat "$1" | grep '^deb-src')" ]; then
        echo "patching classic style $1"
        cat "$1" > /tmp/sources.list.orig
        cat /tmp/sources.list.orig | grep "^deb " | sed 's/^deb /deb-src /g' >> "$1"
    fi
}

install_deps_apt_get() {
    if [ -f "/etc/apt/sources.list" ]; then
        patch_apt_source "/etc/apt/sources.list"
    fi

    if [ -d "/etc/apt/sources.list.d/" ]; then
        for source in $(find /etc/apt/sources.list.d/ -type f); do
            patch_apt_source "$source"
        done
    fi

    # install the dependency
    DEBIAN_FRONTEND=noninteractive apt-get -y update
    DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-recommends install \
        build-essential \
        ca-certificates \
        dpkg-dev \
        gzip \
        git \
        libgd-dev \
        libgeoip-dev \
        libperl-dev \
        libssl-dev \
        libxslt1-dev \
        nginx \
        tar \
        wget \
        zlib1g-dev \
        $@
}

install_deps_pacman() {
    # makepkg will fetch the deps
    pacman -Syy --needed --noconfirm \
        sudo \
        base-devel \
        git \
        perl-rename
}

install_deps_apk() {
    # without --no-cache because it needs to 
    # update sources before calling abuild
    apk add \
        alpine-sdk \
        doas
}

if [ ! -f "/etc/os-release" ]; then
    echo "Missing /etc/os-release"
    exit 1
fi

. /etc/os-release

case $ID in
    ubuntu)
        echo "found debian $VERSION_CODENAME"
        case $VERSION_CODENAME in
            focal)
                install_deps_apt_get "libpcre3-dev"
                ;;
            jammy)
                install_deps_apt_get "libpcre3-dev"
                ;;
            lunar)
                sed -i -re 's/([a-z]{2}\.)?archive.ubuntu.com|security.ubuntu.com/old-releases.ubuntu.com/g' /etc/apt/sources.list
                install_deps_apt_get "libpcre3-dev"
                ;;
            mantic)
                sed -i -re 's/([a-z]{2}\.)?archive.ubuntu.com|security.ubuntu.com/old-releases.ubuntu.com/g' /etc/apt/sources.list
                install_deps_apt_get "libpcre2-dev"
                ;;
            *)
                echo "Not handled."
                exit 1
                ;;
        esac
        ;;
    debian)
        echo "found debian $VERSION_CODENAME"
        case $VERSION_CODENAME in
            bookworm)
                # required to call apt source
                chmod 777 .
                install_deps_apt_get "libpcre3-dev" "libperl-dev" "dpkg-dev"
                ;;
            bullseye)
                install_deps_apt_get "libpcre3-dev" "libperl-dev"
                ;;
            buster)
                install_deps_apt_get "libpcre3-dev" "libperl-dev"
                ;;
            *)
                echo "Not handled."
                exit 1
                ;;
        esac
        ;;
    arch)
        echo "Found arch linux"
        install_deps_pacman
        ;;
    alpine)
        echo "Found alpine linux"
        install_deps_apk
        ;;
    *)
        echo "Unknown OS: $ID"
        exit 1
        ;;
esac
