#!/bin/bash -x
#
# References:
#
#	* https://www.msys2.org/docs/ci/
#
#
echo "Running ${0}"

die ( ) {
	[ "$1" ] && echo "$*"
	exit -1
}

myDIR="$(dirname $(dirname "$(readlink -f "${0}")"))"

cd "${myDIR}"
rm -fr ./.build
mkdir -p ./.build

#
# Unpack LIB3270
#
echo "Unpacking lib3270"
tar -C / -Jxvf "${MINGW_PACKAGE_PREFIX}-lib3270.tar.xz" 

#
# Build LIBIPC3270
#
echo "Building LIBIPC3270"
./autogen.sh || die "Autogen failure"
./configure || die "Configure failure"
make clean || die "Make clean failure"
make all  || die "Make failure"

make DESTDIR=.bin/package install || die "Install failure"
tar --create --xz --file="${MINGW_PACKAGE_PREFIX}-libipc3270.tar.xz" --directory=.bin/package --verbose . || die "Package failure"

echo "Build complete"

