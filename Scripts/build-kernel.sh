#!/bin/bash

LIMINE_URL="https://github.com/limine-bootloader/limine.git"
LIMINE_BRANCH="v3.0-branch-binary"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PATH_THIRDPARTY="$(realpath --relative-to="$SCRIPT_DIR" "$SCRIPT_DIR/../Thirdparty")"
PATH_KERNEL="$(realpath --relative-to="$SCRIPT_DIR" "$SCRIPT_DIR/../Kernel")"
PATH_BASE="$(realpath --relative-to="$SCRIPT_DIR" "$SCRIPT_DIR/../Base")"
PATH_APPS="$(realpath --relative-to="$SCRIPT_DIR" "$SCRIPT_DIR/../Apps")"

# Download and build Limine
echo "Checking for limine..."
if [ ! -d "$PATH_THIRDPARTY/limine" ]; then
    git clone --depth 1 --branch "$LIMINE_BRANCH" "$LIMINE_URL" "$PATH_THIRDPARTY/limine"
    cd "$PATH_THIRDPARTY/limine"
    make
    # make install
fi