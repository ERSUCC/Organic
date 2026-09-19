#!/bin/bash

set -e

VERSION=0.1.0

cmake -B build -DORGANIC_VERSION=$VERSION
cmake --build build --config Release
cmake --install build --config Release --prefix install

patchelf --add-rpath /usr/lib/organic install/bin/organic

TMP_ROOT=/tmp/organic-$VERSION
TMP_INSTALL=$TMP_ROOT/usr

rm -rf $TMP_ROOT
mkdir -p $TMP_INSTALL/bin $TMP_INSTALL/lib/organic

cp install/bin/organic $TMP_INSTALL/bin
cp install/lib/librtaudio*.so* $TMP_INSTALL/lib/organic

mkdir -p $TMP_ROOT/DEBIAN

sed s/{{version}}/$VERSION/ dist/linux/control > $TMP_ROOT/DEBIAN/control

dpkg-deb --build $TMP_ROOT install/organic_0.1.0_amd64.deb

rm -rf $TMP_ROOT
