#!/bin/sh
echo sudo port install binutils libtool awk sed
/configure \
	--with-sed=/opt/local/bin/gsed \
	--with-awk=/opt/local/bin/gawk \
	--with-libtool=/opt/local/bin/glibtool \
	--with-libtoolize=/opt/local/bin/glibtoolize \
	--with-objcopy=/opt/local/bin/gobjcopy \
	--with-objdump=/opt/local/bin/gobjdump \
	--with-readelf=/opt/local/bin/greadelf
