#!/bin/sh
# SPDX-FileCopyrightText: 2023 wargio <deroad@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only
set -e

check_file_for() {
    local FILENAME="$1"
    local SEARCH_FOR="$2"
    local EXPECTED="$3"
    local FOUND=$(cat "$FILENAME" | grep "$SEARCH_FOR")
    if [ -z "$FOUND" ]; then
        echo "'$FILENAME' does not contain '$SEARCH_FOR'."
        exit 1
    fi
    local HAS_EXPECTED=$(echo "$FOUND" | grep "$EXPECTED")
    if [ -z "$HAS_EXPECTED" ]; then
        echo "'$FILENAME' differs with what is expected to have."
        echo "Expected: $EXPECTED"
        echo "Got:      $FOUND"
        exit 1
    fi
}

# check distro release version.
NAXSI_VERSION=$(grep -R "NAXSI_VERSION" naxsi_src/ | cut -d '"' -f2)

if [ -z "$NAXSI_VERSION" ]; then
    echo "Cannot find NAXSI_VERSION in naxsi_src/"
    exit 1
fi

check_file_for "distros/arch/PKGBUILD" "pkgver=" "$NAXSI_VERSION"
check_file_for "distros/alpine/1.18.0-r13/APKBUILD" "_add_module \"http-naxsi" "$NAXSI_VERSION"
check_file_for "distros/alpine/1.20.1-r3/APKBUILD" "_add_module \"http-naxsi" "$NAXSI_VERSION"
check_file_for "distros/alpine/1.20.2-r1/APKBUILD" "_add_module \"http-naxsi" "$NAXSI_VERSION"
check_file_for "distros/alpine/1.22.1-r0/APKBUILD" "_add_module \"http-naxsi" "$NAXSI_VERSION"
check_file_for "distros/alpine/1.18.0-r15/APKBUILD" "_add_module \"http-naxsi" "$NAXSI_VERSION"
check_file_for "distros/alpine/1.20.2-r0/APKBUILD" "_add_module \"http-naxsi" "$NAXSI_VERSION"
check_file_for "distros/alpine/1.22.0-r1/APKBUILD" "_add_module \"http-naxsi" "$NAXSI_VERSION"
check_file_for "distros/alpine/1.24.0-r3/APKBUILD" "_add_module \"http-naxsi" "$NAXSI_VERSION"

# check libinjection hash
SUBMODULE_HASH=$(git -C naxsi_src/libinjection rev-parse HEAD)
for DISTRO_YAML_HASH in $(grep "git -C naxsi_src/libinjection checkout" .github/workflows/distros.yaml | xargs -L1 echo | cut -d ' ' -f5); do
    if [ "$DISTRO_YAML_HASH" != "$SUBMODULE_HASH" ]; then
        echo "The libinjection hash in .github/workflows/distros.yaml does not match the submodule one."
        echo "Expected: $SUBMODULE_HASH"
        echo "Got:      $DISTRO_YAML_HASH"
        echo ".github/workflows/distros.yaml:"
        grep -n "$DISTRO_YAML_HASH" .github/workflows/distros.yaml
        exit 1
    fi
done
