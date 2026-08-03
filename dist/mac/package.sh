#!/bin/bash

set -e

VERSION=0.1.0

cmake -B build -DORGANIC_VERSION=$VERSION
cmake --build build --config Release
cmake --install build --config Release --prefix install

INSTALL_ROOT=/usr/local

install_name_tool -add_rpath $INSTALL_ROOT/lib/organic install/bin/organic

TMP_ROOT=/tmp/organic-$VERSION

rm -rf $TMP_ROOT
mkdir -p $TMP_ROOT/bin $TMP_ROOT/lib/organic

cp install/bin/organic $TMP_ROOT/bin
cp install/lib/*.dylib $TMP_ROOT/lib/organic

pkgbuild --root $TMP_ROOT --identifier Organic --version $VERSION --install-location $INSTALL_ROOT "$TMP_ROOT/OrganicExec.pkg"
productbuild --distribution dist/mac/distribution.xml --package-path $TMP_ROOT "install/Organic $VERSION.pkg"

rm -rf $TMP_ROOT
