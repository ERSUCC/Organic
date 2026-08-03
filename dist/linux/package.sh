#!/bin/bash

set -e

VERSION=0.1.0

cmake -B build -DORGANIC_VERSION=$VERSION
cmake --build build --config Release
cmake --install build --config Release --prefix install

INSTALL_ROOT=/usr/local

patchelf --add-rpath $INSTALL_ROOT/lib/organic install/bin/organic

TMP_ROOT=/tmp/organic_$VERSION
TMP_INSTALL=$TMP_ROOT$INSTALL_ROOT

rm -rf $TMP_ROOT
mkdir -p $TMP_INSTALL/bin $TMP_INSTALL/lib/organic

cp install/bin/organic $TMP_INSTALL/bin
cp install/lib/*.so* $TMP_INSTALL/lib/organic

mkdir -p $TMP_ROOT/DEBIAN

sed s/{{version}}/$VERSION/ dist/linux/control > $TMP_ROOT/DEBIAN/control

dpkg-deb --build $TMP_ROOT install/organic.deb

rm -rf $TMP_ROOT
