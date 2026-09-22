#!/bin/bash

set -e

VERSION=0.1.0

cmake -B build -DORGANIC_VERSION=$VERSION
cmake --build build --config Release
cmake --install build --config Release --prefix install

patchelf --add-rpath /usr/lib64/organic install/bin/organic

ARCHIVE=organic-$VERSION
TMP_ROOT=/tmp/$ARCHIVE
TMP_INSTALL=$TMP_ROOT/$ARCHIVE
RPM_ROOT=$TMP_ROOT/rpmbuild

rm -rf $TMP_ROOT
mkdir -p $TMP_INSTALL/bin $TMP_INSTALL/lib64/organic $RPM_ROOT/SOURCES $RPM_ROOT/SPECS

cp install/bin/organic $TMP_INSTALL/bin
cp install/lib/librtaudio*.so* $TMP_INSTALL/lib64/organic

tar -cf $RPM_ROOT/SOURCES/$ARCHIVE.tar.gz -C $TMP_ROOT $ARCHIVE

sed s/{{version}}/$VERSION/ dist/linux/organic.spec > $RPM_ROOT/SPECS/organic.spec

SOURCE_DATE_EPOCH=$(date +%s) rpmbuild --define "_topdir $RPM_ROOT" -bb $RPM_ROOT/SPECS/organic.spec

cp $RPM_ROOT/RPMS/x86_64/*.rpm install

rm -rf $TMP_ROOT
