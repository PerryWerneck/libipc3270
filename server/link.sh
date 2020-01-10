#!/bin/bash

make clean
if [ "$?" != "0" ]; then
	exit -1
fi

make Debug
if [ "$?" != "0" ]; then
	exit -1
fi

sudo ln -sf $(readlink -f ../.bin/Debug/ipcserver.so) /usr/lib64/$(pkg-config --variable=product_name lib3270)-plugins/ipcserver.so

