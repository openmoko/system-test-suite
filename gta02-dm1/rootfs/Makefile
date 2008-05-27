
CROSS=arm-angstrom-linux-gnueabi

BUSYBOX=busybox-1.10.2
BUSYBOX_HOST=http://busybox.net/downloads
BUSYBOX_SRC=${BUSYBOX}.tar.bz2

#GSM=gsm

all: target_rootfs_base etc_data_install dev_node_build \
	download_src busybox_build

clean: busybox_clean dev_node_clean etc_data_remove

distclean: clean
	@if [ -d ${BUSYBOX} ]; then \
		rm -fr ${BUSYBOX}; \
	fi && \
	if [ -d target_rootfs ]; then \
		rm -fr target_rootfs; \
	fi && \
	if [ -d sources ]; then \
		rm -fr sources; \
	fi

target_rootfs_base:
	@if [ ! -d target_rootfs ]; then \
		mkdir target_rootfs; \
	fi && cd target_rootfs && \
	mkdir -p bin etc lib proc sys dev dev/input dev/pts sbin \
		 mnt sys usr boot home media opt root srv var tmp

etc_data_install:
	@cp -frp target_rootfs_data/etc target_rootfs/

etc_data_remove:
	@rm -fr target_rootfs/etc/*

download_src:
	@if [ ! -d sources ]; then \
		mkdir sources; \
	fi
	@if [ ! -e sources/${BUSYBOX_SRC} ]; then \
		wget ${BUSYBOX_HOST}/${BUSYBOX_SRC} \
		-O sources/${BUSYBOX_SRC}; \
	fi
	#if [ ! -d sources/${GSM} ]; then \
	#	cd sources && \
	#	svn co http://svn.openmoko.org/trunk/src/target/${GSM}; \
	#fi

busybox_build:
	@if [ ! -d ${BUSYBOX} ]; then \
		tar -xjf sources/${BUSYBOX_SRC}; \
	fi
	@cd ${BUSYBOX} && cp ../config/def-busybox-config ./.config && \
	echo ${PWD} | sed "s/\//\\\\\//g" - \
		| sed "s/\$$/\\\\\/target_rootfs/g" - > path.txt && \
	sed "s/.\/_install/`cat path.txt`/g" .config > .config_done && \
	mv .config_done .config && rm path.txt && \
	make ARCH=arm CROSS_COMPILE=${CROSS}- oldconfig && \
	make ARCH=arm CROSS_COMPILE=${CROSS}- -j 3 && \
	make ARCH=arm CROSS_COMPILE=${CROSS}- install
	@cd target_rootfs && ln -s sbin/init init

busybox_clean:
	@cd ${BUSYBOX} && make clean
	@rm -f target_rootfs/init

#gsm_build:
#	if [ ! -d ${GSM} ]; then \
#		cp -frp sources/${GSM} ./ ; \
#	fi
#	cd ${GSM} && \
#	if [ ! -e .patched_done ]; then \
#		patch -p0 < ../sources/gsmd-dm2.patch && \
#		touch .patched_done; \
#	fi && \
#	./autogen.sh && \
#	./configure CFLAGS="-g -O2 -static" --host=${CROSS} \
#		--prefix=/usr --enable-static --disable-shared && \
#	make CFLAGS="-g -O2 -static" -j 3

dev_node_build:
	@cd target_rootfs/dev && \
	echo "##### next, maybe you input password for sudo: mknod #####" && \
	sudo mknod console c 5 1 && \
	sudo mknod fb0 c 29 0 && \
	sudo mknod input/event0 c 13 64 && \
	sudo mknod input/event1 c 13 65 && \
	sudo mknod input/event2 c 13 66 && \
	sudo mknod mem c 1 1 && \
	sudo mknod mtdblock0 c 31 0 && \
	sudo mknod null c 1 3 && \
	sudo mknod ptmx c 5 2 && \
	sudo mknod tty0 c 4 0 && \
	sudo mknod tty1 c 4 1 && \
	sudo mknod tty2 c 4 2 && \
	sudo mknod tty5 c 4 5 && \
	sudo mknod ttySAC0 c 204 64 && \
	sudo mknod ttySAC1 c 204 65 && \
	sudo mknod ttySAC2 c 204 66

dev_node_clean:
	@cd target_rootfs/dev && \
	echo "##### next, maybe you input password for sudo: rm #####" && \
	sudo rm -f console fb0 input/event[0-2] mem mtdblock0 null ptmx \
			tty[0125] ttySAC[0-2]

