
#Please Setting this...
GTA02_DM_RELEASE_VERSION = ""

BASE_DIR = ${PWD}
GTA02_DM1_DIR = ${BASE_DIR}/gta02-dm1
GTA02_DM1_TARGET = GTA02-DM1-${GTA02_DM_RELEASE_VERSION}.tar.gz
GTA02_DM2_DIR = ${BASE_DIR}/gta02-dm2
GTA02_DM2_TARGET = dm2.bin
GTA02_RELEASE_DIR = ${BASE_DIR}/release_data
GTA02_VERSION_FILE = ${BASE_DIR}/setting_dm_version

#BI_KERNEL_VER = 2.6.22.5
#BI_KERNEL_SITE = http://www.kernel.org
#DL_KERNEL_SRC = ${BI_KERNEL_SITE}/pub/linux/kernel/v2.6/linux-${BI_KERNEL_VER}.tar.bz2 

export GTA02_VERSION_FILE ;

GTA02_CROSS = "arm-angstrom-linux-gnueabi-"

#all: verify_version_setup download_git gsm_utility_build gta02-dm1-dummy gta02-dm2
all: verify_version_setup download_git gsm_utility_build gta02-dm2

# test #########

.PHONY: verify_version_setup
verify_version_setup:
	@if [ -z ${GTA02_DM_RELEASE_VERSION} ]; then \
		echo "" && \
		echo "\tERROR!!! Please Setting GTA02_DM_RELEASE_VERSION in Makefile" && \
		echo "" && \
		false; \
	fi
	@echo ${GTA02_DM_RELEASE_VERSION} > ${GTA02_VERSION_FILE}
	@echo ${GTA02_DM_RELEASE_VERSION} > ./gta02-dm2/RELEASE_STRING

.PHONY: dump_setting
dump_setting: 
	@echo ""
	@echo "GTA02_DM_RELEASE_VERSION = "${GTA02_DM_RELEASE_VERSION}
	@echo ""
	@echo "BASE_DIR = "${BASE_DIR}
	@echo "GTA02_DM1_DIR = "${GTA02_DM1_DIR}
	@echo "GTA02_DM1_TARGET = "${GTA02_DM1_TARGET}
	@echo "GTA02_DM2_DIR = "${GTA02_DM2_DIR}
	@echo "GTA02_DM2_TARGET = "${GTA02_DM2_TARGET}
	@echo ""
	@echo "PATH = "${PATH}
	@echo ""

# all ##########
download_git:
	if [ ! -d ${BASE_DIR}/download_git ]; then \
		mkdir download_git; \
	fi 
	cd ${BASE_DIR}/download_git && \
	if [ ! -d linux-2.6 ]; then \
		git clone git://git.openmoko.org/git/kernel.git linux-2.6; \
	fi && cd linux-2.6 && \
	if [ ! -e ../.checkout_ver_andy_kernel ]; then \
		git checkout origin/andy && \
		touch ../.checkout_ver_andy_kernel; \
	fi 
	cd ${BASE_DIR}/download_git && \
	if [ ! -d u-boot ]; then \
		git clone git://git.openmoko.org/git/u-boot.git u-boot; \
	fi && cd u-boot && \
	if [ ! -e ../.checkout_ver_andy_uboot ]; then \
		git checkout origin/andy && \
		touch ../.checkout_ver_andy_uboot; \
	fi

.PHONY: gsm_utility_build
gsm_utility_build:
	@cd ${BASE_DIR}/gsm-utility && \
	if [ ! -d gsm ]; then \
		svn co http://svn.openmoko.org/trunk/src/target/gsm; \
	fi && \
	cd gsm && \
	if [ ! -e .patched_done ]; then \
		patch -p0 < ../gsmd-dm2.patch && \
		touch .patched_done; \
	fi && \
	./autogen.sh && mkdir -p build && \
	./configure --host=arm-angstrom-linux-gnueabi --prefix=/usr && \
	make && make DESTDIR=${PWD}/gsm-utility/build install


.PHONY: gta02-dm1-mkdir-tmp
gta02-dm1-mkdir-tmp:
	@if [ ! -d ${GTA02_DM1_DIR}/tmp ]; then \
		mkdir ${GTA02_DM1_DIR}/tmp; \
	fi

.PHONY: gta02-dm1-build-kernel-include-rootfs
gta02-dm1-build-kernel-include-rootfs: gta02-dm1-mkdir-tmp
	cd ${GTA02_DM1_DIR}/rootfs && make && \
	cd ${GTA02_DM1_DIR}/user_space && make && \
	echo "##### maybe you need input password for sudo #####" && \
	sudo cp main \
		${GTA02_DM1_DIR}/rootfs/target_rootfs/sbin && \
	cd ${GTA02_DM1_DIR}/rootfs/target_rootfs && \
	cp -fr ${BASE_DIR}/gsm-utility/build/* ./ && \
	cd ${GTA02_DM1_DIR}/tmp && \
	if [ ! -d linux-2.6 ]; then \
		cp -fr ${BASE_DIR}/download_git/linux-2.6 ./ ; \
	fi && \
	cp ./linux-2.6/defconfig-2.6.24 ./linux-2.6/.config && \
	sed "/CONFIG_INITRAMFS_SOURCE/c\CONFIG_INITRAMFS_SOURCE=\"_SETTING_NEW_PATH_TO_ME_\"" ./linux-2.6/.config | \
	sed "/CONFIG_INITRAMFS_SOURCE/a\CONFIG_INITRAMFS_ROOT_UID=0\nCONFIG_INITRAMFS_ROOT_GID=0" - | \
	sed "/CONFIG_SND_S3C24XX_SOC_NEO1973_WM8753/a\# CONFIG_SND_S3C24XX_SOC_NEO1973_WM8753_DEBUG is not set" - > ./linux-2.6/.config_pre_setting && \
	echo ${PWD} | sed "s/\//\\\\\//g" - \
                | sed "s/\$$/\\\\\/gta02-dm1\\\\\/rootfs\\\\\/target_rootfs/g" - \
		> path.txt && \
        sed "s/_SETTING_NEW_PATH_TO_ME_/`cat path.txt`/g" ./linux-2.6/.config_pre_setting > \
		./linux-2.6/.config_done && rm path.txt ./linux-2.6/.config_pre_setting && \
	mv ./linux-2.6/.config_done ./linux-2.6/.config && cd linux-2.6 && \
	export PATH=${PATH}:${GTA02_DM1_DIR}/tmp/u-boot/tools && \
	make ARCH=arm CROSS_COMPILE=${GTA02_CROSS} uImage -j 4 && \
	${GTA02_CROSS}objcopy -O binary -R .note -R .comment \
		-S arch/arm/boot/compressed/vmlinux linux.bin && \
	mkimage -A arm -O linux -T kernel -C none -a 30008000 \
		-e 30008000 -n "Openmoko Kernel Image Neo1973(GTA02)" \
		-d linux.bin uImage.bin && \
	if [ ! -e uImage.bin ]; then \
		echo "" && \
		echo "##### ${PWD}/uImage.bin is not Built Done #####" && \
		echo "" && \
		false; \
	fi
	@echo "" 
	@echo " ##### kernel and include rootfs is Ready!! #####" 
	@echo ""

.PHONY: gta02-dm1-build-nor-uboot 
gta02-dm1-build-nor-uboot: gta02-dm1-mkdir-tmp
	@cd ${GTA02_DM1_DIR}/tmp && \
	if [ ! -e ./devirginator/.svn/entries ]; then \
		svn co http://svn.openmoko.org/trunk/src/host/devirginator; \
	fi && \
	if [ ! -e ./splash/.svn/entries ]; then \
		svn co http://svn.openmoko.org/trunk/src/host/splash; \
	fi && \
	if [ ! -e ./u-boot/.git/HEAD ]; then \
		cp -fr ${BASE_DIR}/download_git/u-boot ./ && \
		cd u-boot && \
		cd ${GTA02_DM1_DIR}/tmp; \
	fi && \
	if [ ! -e ./devirginator/System_boot.png ]; then \
		cd ./devirginator && \
		wget http://wiki.openmoko.org/images/c/c2/System_boot.png; \
	fi
	@cd ${GTA02_DM1_DIR}/tmp/u-boot && \
	if [ ! -e ${GTA02_DM1_DIR}/tmp/u-boot/.config_done ]; then \
		make ARCH=arm CROSS_COMPILE=${GTA02_CROSS} gta02v5_config && \
		touch ${GTA02_DM1_DIR}/tmp/u-boot/.config_done; \
	fi && \
	make ARCH=arm CROSS_COMPILE=${GTA02_CROSS} u-boot.udfu -j 2 && \
	cd ${GTA02_DM1_DIR}/tmp/devirginator && \
	./mknor -D QUIET -s ./System_boot.png \
		-o ../u-boot/u-boot.udfu.nor ../u-boot/u-boot.udfu
	@echo "" 
	@echo " ##### u-boot.udfu.nor for NOR Ready!! #####" 
	@echo ""

.PHONY: gta02-dm1
gta02-dm1: gta02-dm1-mkdir-tmp gta02-dm1-build-nor-uboot \
		gta02-dm1-build-kernel-include-rootfs
	@echo "##### build ${GTA02_DM1_TARGET} #####"
	@echo ""
	@if [ ! -e ${GTA02_VERSION_FILE} ]; then \
		echo "" && \
		echo "\tERROR!!! Please Setting GTA02_DM_RELEASE_VERSION in Makefile" && \
		echo "" && \
		false; \
	fi
	@cd ${GTA02_DM1_DIR}/tmp && \
	tar -xzf ../GTA02-P-version.tar.gz && \
	echo ${GTA02_DM_RELEASE_VERSION} > GTA02-DM1/version && \
	cp ${GTA02_DM1_DIR}/tmp/linux-2.6/uImage.bin \
		GTA02-DM1/tmp/uImage.bin && \
	cp ${GTA02_DM1_DIR}/tmp/u-boot/u-boot.udfu \
		./GTA02-DM1/tmp/u-boot.bin && \
	echo "I am Live" > GTA02-DM1/.make_identify && \
	tar -czf ${GTA02_DM1_DIR}/${GTA02_DM1_TARGET} \
		GTA02-DM1 && \
	cd ${BASE_DIR}

.PHONY: gta02-dm1-dummy
gta02-dm1-dummy:
	@echo "##### build ${GTA02_DM1_TARGET} #####"
	@echo ""
	@if [ ! -e ${GTA02_VERSION_FILE} ]; then \
		echo "" && \
		echo "\tERROR!!! Please Setting GTA02_DM_RELEASE_VERSION in Makefile" && \
		echo "" && \
		false; \
	fi
	@if [ ! -d ${GTA02_DM1_DIR}/tmp ]; then \
		mkdir ${GTA02_DM1_DIR}/tmp; \
	fi
	rm -fr ${GTA02_DM1_DIR}/tmp/GTA02-DM1 && \
	cd ${GTA02_DM1_DIR}/tmp && \
	tar -xzf ../GTA02-P-version.tar.gz && \
	echo ${GTA02_DM_RELEASE_VERSION} > GTA02-DM1/version && \
	cp GTA02-DM1/tmp/u-boot.bin.Ver_0.0.27 GTA02-DM1/tmp/u-boot.bin && \
	cp GTA02-DM1/tmp/uImage.bin.Ver_0.0.27 GTA02-DM1/tmp/uImage.bin && \
	echo "I am Dummy" > GTA02-DM1/.make_identify && \
	tar -czf ${GTA02_DM1_DIR}/${GTA02_DM1_TARGET} \
		GTA02-DM1 && \
	cd ${BASE_DIR}

.PHONY: gta02-dm2
gta02-dm2: 
	@echo "##### build ${GTA02_DM2_TARGET} #####"
	@if [ ! -e ${GTA02_VERSION_FILE} ]; then \
		echo "" && \
		echo "\tERROR!!! Please Setting GTA02_DM_RELEASE_VERSION in Makefile" && \
		echo "" && \
		false; \
	fi
	@cd ${GTA02_DM2_DIR} && ./build && cd ${BASE_DIR} 


# install ##########
.PHONY: mkdir_release_dir
mkdir_release_dir:
	@if [ ! -d ${GTA02_RELEASE_DIR} ]; then \
		mkdir ${GTA02_RELEASE_DIR}; \
	fi
	@if [ ! -d ${GTA02_RELEASE_DIR}/gta02-dm2 ]; then \
		mkdir ${GTA02_RELEASE_DIR}/gta02-dm2; \
	fi

.PHONY: install-gta02-dm1
install-gta02-dm1: mkdir_release_dir
	@if [ -e ${GTA02_DM1_DIR}/${GTA02_DM1_TARGET} ]; then \
		mv ${GTA02_DM1_DIR}/${GTA02_DM1_TARGET} \
			${GTA02_RELEASE_DIR}/${GTA02_DM1_TARGET}; \
	fi

.PHONY: install-gta02-dm2
install-gta02-dm2: mkdir_release_dir
	@if [ -e ${GTA02_DM2_DIR}/staging/usr/bin/${GTA02_DM2_TARGET} ]; then \
		cp -frp ${GTA02_DM2_DIR}/staging/* \
			${GTA02_RELEASE_DIR}/gta02-dm2 && \
		cp ${GTA02_DM2_DIR}/staging/usr/bin/${GTA02_DM2_TARGET} \
			${GTA02_RELEASE_DIR}/${GTA02_DM2_TARGET}; \
	fi


install: mkdir_release_dir install-gta02-dm1 install-gta02-dm2

# clean ##########
.PHONY: gta02-dm1-build-kernel-include-rootfs-clean
gta02-dm1-build-kernel-include-rootfs-clean:
	cd ${GTA02_DM1_DIR}/user_space && make clean && \
	if [ -d ${GTA02_DM1_DIR}/tmp/rootfs ]; then \
		sudo rm -fr ${GTA02_DM1_DIR}/tmp/rootfs; \
	fi
	cd ${GTA02_DM1_DIR}/tmp/linux-2.6 && \
	make ARCH=arm CROSS_COMPILE=${GTA02_CROSS} clean

.PHONY: gta02-dm1-build-nor-uboot-clean
gta02-dm1-build-nor-uboot-clean:
	if [ -e ${GTA02_DM1_DIR}/tmp/u-boot/u-boot ]; then \
		cd ${GTA02_DM1_DIR}/tmp/u-boot && \
		make ARCH=arm CROSS_COMPILE=${GTA02_CROSS} distclean && \
		cd - ; \
	fi 
	if [ -e ${GTA02_DM1_DIR}/tmp/u-boot/u-boot.udfu ]; then \
		rm ${GTA02_DM1_DIR}/tmp/u-boot/u-boot.udfu; \
	fi
	if [ -e ${GTA02_DM1_DIR}/tmp/u-boot/u-boot.udfu.nor ]; then \
		rm ${GTA02_DM1_DIR}/tmp/u-boot/u-boot.udfu.nor; \
	fi


.PHONY: clean-release-data
clean-release-data:
	@if [ -d ${GTA02_RELEASE_DIR} ]; then \
		rm -fr ${GTA02_RELEASE_DIR}/* && \
		rmdir ${GTA02_RELEASE_DIR}; \
	fi

.PHONY: clean-gta02-dm1
clean-gta02-dm1: gta02-dm1-build-kernel-include-rootfs-clean \
		gta02-dm1-build-nor-uboot-clean clean-gta02-dm1-dummy
	cd ${GTA02_DM1_DIR}/rootfs && make clean

.PHONY: clean-gta02-dm1-dummy
clean-gta02-dm1-dammy:
	rm ${GTA02_DM1_DIR}/${GTA02_DM1_TARGET} && \
	rm -fr ${GTA02_DM1_DIR}/tmp/GTA02-DM1 
	@echo "##### clean dm1 #####"
	@echo ""

.PHONY: clean-gta02-dm2
clean-gta02-dm2:
	@echo "##### clean ${GTA02_DM2_TARGET} #####"
	@cd ${GTA02_DM2_DIR} && make clean && rm -fr staging \
		&& cd ${BASE_DIR} 

clean: clean-release-data clean-gta02-dm1-dummy clean-gta02-dm2
	@if [ -e ${GTA02_VERSION_FILE} ]; then \
		rm ${GTA02_VERSION_FILE}; \
	fi

