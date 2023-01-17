#!/bin/bash -x
#
# References:
#
#	* https://www.msys2.org/docs/ci/
#
#
echo "Running ${0}"

LOGFILE=build.log
rm -f ${LOGFILE}

die ( ) {
	[ -s $LOGFILE ] && tail $LOGFILE
	[ "$1" ] && echo "$*"
	exit -1
}

myDIR=$(dirname $(dirname $(readlink -f ${0})))

cd ${myDIR}
rm -fr ./.build
mkdir -p ./.build

#
# Build LIB3270
#
echo "Building lib3270"
git clone https://github.com/PerryWerneck/lib3270.git ./.build/lib3270 > $LOGFILE 2>&1 || die "clone lib3270 failure"
cd ./.build/lib3270
./autogen.sh > $LOGFILE 2>&1 || die "Autogen failure"
./configure > $LOGFILE 2>&1 || die "Configure failure"
make clean > $LOGFILE 2>&1 || die "Make clean failure"
make all  > $LOGFILE 2>&1 || die "Make failure"
make install  > $LOGFILE 2>&1 || die "Install failure"
cd ../..

#
# Build LIBV3270
#
echo "Building libv3270"
mkdir -p  ./.build/libv3270
git clone https://github.com/PerryWerneck/libv3270.git ./.build/libv3270 > $LOGFILE 2>&1 || die "clone libv3270 failure"
cd ./.build/libv3270
./autogen.sh > $LOGFILE 2>&1 || die "Autogen failure"
./configure > $LOGFILE 2>&1 || die "Configure failure"
make clean > $LOGFILE 2>&1 || die "Make clean failure"
make all  > $LOGFILE 2>&1 || die "Make failure"
make install  > $LOGFILE 2>&1 || die "Install failure"
cd ../..

#
# Build IPC3270
#
echo "Building IPC"
./autogen.sh > $LOGFILE 2>&1 || die "Autogen failure"
./configure > $LOGFILE 2>&1 || die "Configure failure"
make clean > $LOGFILE 2>&1 || die "Make clean failure"
make all  > $LOGFILE 2>&1 || die "Make failure"

echo "Build complete"

