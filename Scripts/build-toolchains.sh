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