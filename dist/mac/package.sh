#!/bin/bash

set -e

VERSION=0.1.0

cmake -B build -DORGANIC_VERSION=$VERSION
cmake --build build --config Release
cmake --install build --config Release --prefix install

./build/unify install/bin/organic install/lib

install_name_tool -add_rpath @executable_path/../lib install/bin/organic

codesign -s - -f install/lib/*.dylib

INSTALL_ROOT=/opt/organic
TMP_ROOT=/tmp/organic-$VERSION

rm -rf $TMP_ROOT
mkdir -p $TMP_ROOT/bin $TMP_ROOT/lib

cp install/bin/organic $TMP_ROOT/bin
cp install/lib/*.dylib $TMP_ROOT/lib

pkgbuild --root $TMP_ROOT --identifier OrganicExec --version $VERSION --install-location $INSTALL_ROOT /tmp/OrganicExec.pkg

rm -rf $TMP_ROOT
mkdir -p $TMP_ROOT/paths.d

echo $INSTALL_ROOT/bin > $TMP_ROOT/paths.d/organic

pkgbuild --root $TMP_ROOT --identifier OrganicPath --version $VERSION --install-location /etc /tmp/OrganicPath.pkg

productbuild --distribution dist/mac/distribution.xml --package-path /tmp "install/Organic $VERSION.pkg"

rm -rf $TMP_ROOT
