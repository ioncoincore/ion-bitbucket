#!/usr/bin/env bash
#
# Copyright (c) 2018 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

#DOCKER_EXEC echo \> \$HOME/.ioncoin  # Make sure default datadir does not exist and is never read by creating a dummy file

mkdir -p depends/SDKs depends/sdk-sources

if [ -n "$OSX_SDK" -a ! -f depends/sdk-sources/MacOSX${OSX_SDK}.sdk.tar.xz ]; then
  curl --location --fail $SDK_URL/MacOSX${OSX_SDK}.sdk.tar.xz -o depends/sdk-sources/MacOSX${OSX_SDK}.sdk.tar.xz
fi
if [ -n "$OSX_SDK" -a -f depends/sdk-sources/MacOSX${OSX_SDK}.sdk.tar.xz ]; then
  # Workaround to get the log file smaller as it fails travis because log.txt becomes to big
  #  ref https://github.com/ioncoincore/ion/commit/f01dc2bb837cca34b38a267b84326359aaca3615
  #  Info about workaround: grep something over both the STDOUT and STDERR of tar
  #
  #  Example: tar -C depends/SDKs -xJf depends/sdk-sources/MacOSX${OSX_SDK}.sdk.tar.xz 2>/dev/null
  tar -C depends/SDKs -xJf depends/sdk-sources/MacOSX${OSX_SDK}.sdk.tar.xz 2>/dev/null
fi
if [[ $HOST = *-mingw32 ]]; then
  DOCKER_EXEC update-alternatives --set $HOST-g++ \$\(which $HOST-g++-posix\)
fi
if [ -z "$NO_DEPENDS" ]; then
  DOCKER_EXEC CONFIG_SHELL= make $MAKEJOBS -C depends HOST=$HOST $DEP_OPTS
fi

