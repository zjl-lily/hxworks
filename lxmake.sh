#!/bin/sh

echo "make clean"
make clean

echo "start make lxremoteCtrl !!!!!!!!!!"
#echo "make -f Makefile.lx"
make
#make -f Makefile.lx

echo "cp lxremoteCtrl /var/tftpboot/lxtest165"
cp lxremoteCtrl /var/tftpboot/lxtest165

