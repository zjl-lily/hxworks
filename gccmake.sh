#!/bin/sh

echo "make clean"
make clean

echo "make -f Makefile.g++"

make -f Makefile.g++

ls -l
#echo "cp remoteCtrl  /var/tftpboot/"
#cp remoteCtrl /var/tftpboot/
