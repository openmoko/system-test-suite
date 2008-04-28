
#Please Setting this...
GTA02_DM_RELEASE_VERSION = ""

BASE_DIR = ${PWD}
GTA02_DM1_DIR = ${BASE_DIR}/gta02-dm1
GTA02_DM1_TARGET = GTA02-P${GTA02_DM_RELEASE_VERSION}.tar.gz
GTA02_DM2_DIR = ${BASE_DIR}/gta02-dm2
GTA02_DM2_TARGET = dm2.bin
GTA02_RELEASE_DIR = ${BASE_DIR}/release_data
GTA02_VERSION_FILE = ${BASE_DIR}/setting_dm_version
export GTA02_VERSION_FILE ;

all: verify_version_setup gta02-dm1-dummy gta02-dm2

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
	@rm -fr ${GTA02_DM1_DIR}/tmp/* && \
	cd ${GTA02_DM1_DIR}/tmp && \
	tar -xzf ../GTA02-P-version.tar.gz && \
	echo ${GTA02_DM_RELEASE_VERSION} > GTA02-DM1/version && \
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

.PHONY: install-gta02-dm1-dammy
install-gta02-dm1-dammy: mkdir_release_dir
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

install: mkdir_release_dir install-gta02-dm1-dammy install-gta02-dm2

# clean ##########
.PHONY: clean-release-data
clean-release-data:
	@if [ -d ${GTA02_RELEASE_DIR} ]; then \
		rm -fr ${GTA02_RELEASE_DIR}/* && \
		rmdir ${GTA02_RELEASE_DIR}; \
	fi

.PHONY: clean-gta02-dm1-dammy
clean-gta02-dm1-dammy:
	@echo "##### clean dm1 #####"
	@echo ""
	@if [ -d ${GTA02_DM1_DIR}/tmp ]; then \
		rm -fr ${GTA02_DM1_DIR}/tmp; \
	fi

.PHONY: clean-gta02-dm2
clean-gta02-dm2:
	@echo "##### clean ${GTA02_DM2_TARGET} #####"
	@cd ${GTA02_DM2_DIR} && make clean && rm -fr staging \
		&& cd ${BASE_DIR} 

clean: clean-release-data clean-gta02-dm1-dammy clean-gta02-dm2
	@if [ -e ${GTA02_VERSION_FILE} ]; then \
		rm ${GTA02_VERSION_FILE}; \
	fi

