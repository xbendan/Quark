#!/bin/bash

ARCH=${ARCH:="x86_64"}
MAKE=${MAKE:="make"}

GCC_VERSION="14.2.0"
GCC_MD5SUM="b89ddcdaf5c1b6214abad40d9761a6ba"
GCC_NAME="gcc-${GCC_VERSION}"
GCC_PACKAGE="${GCC_NAME}.tar.gz"
GCC_URL="https://ftp.gnu.org/gnu/gcc/${GCC_NAME}/${GCC_PACKAGE}"
BINUTILS_VERSION="2.43.1"
BINUTILS_MD5SUM="7aaf8e17f2dfebabcf1d1ac103f7c149"
BINUTILS_NAME="binutils-${BINUTILS_VERSION}"
BINUTILS_PACKAGE="${BINUTILS_NAME}.tar.gz"
BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/${BINUTILS_PACKAGE}"

# Download and extract the GCC package
if [ ! -f "${GCC_PACKAGE}" ]; then
    wget "${GCC_URL}"
fi

if [ ! -f "${GCC_PACKAGE}" ]; then
    echo "Failed to download GCC package"
    exit 1
fi

if [ ! -d "${GCC_NAME}" ]; then
    tar -xvf "${GCC_PACKAGE}"
fi

if [ ! -d "${GCC_NAME}" ]; then
    echo "Failed to extract GCC package"
    exit 1
fi

# Download and extract the Binutils package

if [ ! -f "${BINUTILS_PACKAGE}" ]; then
    wget "${BINUTILS_URL}"
fi

if [ ! -f "${BINUTILS_PACKAGE}" ]; then
    echo "Failed to download Binutils package"
    exit 1
fi

if [ ! -d "${BINUTILS_NAME}" ]; then
    tar -xvf "${BINUTILS_PACKAGE}"
fi

if [ ! -d "${BINUTILS_NAME}" ]; then
    echo "Failed to extract Binutils package"
    exit 1
fi