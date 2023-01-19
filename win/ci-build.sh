#!/bin/bash -x
#
# References:
#
#	* https://www.msys2.org/docs/ci/
#
#
echo "Running ${0}"

myDIR=$(dirname $(dirname $(readlink -f ${0})))

cd ${myDIR}
rm -fr ./.build
mkdir -p ./.build

#
# Build LIB3270
#
echo "Building lib3270"
git clone https://github.com/PerryWerneck/lib3270.git ./.build/lib3270 || die "clone lib3270 failure"
cd ./.build/lib3270
./autogen.sh || die "Lib3270 autogen failure"
./configure || die "Lib3270 Configure failure"
make clean || die "Lib3270 Make clean failure"
make all || die "Lib3270 Make failure"
make install || die "Lib3270 Install failure"
cd ../..

#
# Build LIBIPC3270
#
echo "Building LIBIPC3270"
./autogen.sh || die "Autogen failure"
./configure || die "Configure failure"
make clean || die "Make clean failure"
make all  || die "Make failure"

echo "Build complete"

