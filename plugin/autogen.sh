#!/bin/bash

builddir=${PWD}

NOCONFIGURE=1 ./plugin/autogen.sh
if test $? != 0 ; then
        echo "plugin autogen failed."
        exit -1
fi

NOCONFIGURE=1 ./client/autogen.sh
if test $? != 0 ; then
        echo "client autogen failed."
        exit -1
fi

test -n "$srcdir" || srcdir=$(readlink -f $(dirname "$0"))
test -n "$srcdir" || srcdir=$(readlink -f .)

cd ${srcdir}

mkdir -p ./scripts
mkdir -p m4

libtoolize --force
if test $? != 0 ; then
        echo "libtoolize failed."
        exit -1
fi

aclocal
if test $? != 0 ; then
	echo "aclocal failed."
	exit -1
fi

autoheader --force
if test $? != 0 ; then
       echo "autoheader failed."
       exit -1
fi

autoconf --force
if test $? != 0 ; then
	echo "autoconf failed."
	exit -1
fi

automake --add-missing 2> /dev/null | true

autopoint

cd ${builddir}
	
test -n "$NOCONFIGURE" || "./configure" "$@"



