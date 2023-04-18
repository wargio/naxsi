#!/bin/sh
# SPDX-FileCopyrightText: 2023 wargio <deroad@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only
set -e

DISTRO_YAML_HASH=$(cat .github/workflows/distros.yaml | grep "git -C naxsi_src/libinjection checkout" | xargs echo | cut -d ' ' -f5)
SUBMODULE_HASH=$(git submodule status | xargs echo | cut -d ' ' -f1)

if [ "$DISTRO_YAML_HASH" != "$SUBMODULE_HASH" ]; then
    echo "The libinjection hash in .github/workflows/distros.yaml does not match the submodule one."
    echo "Expected: $SUBMODULE_HASH"
    echo "Got:      $DISTRO_YAML_HASH"
    echo ".github/workflows/distros.yaml:"
    grep -n "$DISTRO_YAML_HASH" .github/workflows/distros.yaml
    exit 1
fi
