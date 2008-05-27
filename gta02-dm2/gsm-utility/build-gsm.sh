#!/bin/sh

if [ ! -d gsm ]; then
	svn co http://svn.openmoko.org/trunk/src/target/gsm;
fi && \
cd gsm && \
if [ ! -e .patched_done ]; then
	patch -p0 < ../gsmd-dm2.patch && \
	touch .patched_done;
fi && \
./autogen.sh && mkdir -p build && \
./configure --host=arm-angstrom-linux-gnueabi --prefix=/usr && \
make && make DESTDIR=${PWD}/build install 
