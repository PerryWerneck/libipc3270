#!/bin/bash
make Debug 

sudo ln -sf $(readlink -f ../.bin/Debug/libipc3270.so.5.3) /usr/lib64/libipc3270.so.5.3



