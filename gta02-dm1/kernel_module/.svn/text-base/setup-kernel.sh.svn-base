#! /usr/bin/env bash
# Patch and setup kernel for DM.
#
# Copyright (C) 2007 OpenMoko, Inc.
# Written by Jim Huang <jserv@openmoko.org>
# All Rights Reserved

abort () {
	echo -e patcher: $@
	exec false
}

SVN_BASEDIR=`pwd`

# check parameter
if test x"$1" = x; then
	abort "no kernel directory given.\nUsage: setup-kernel.sh <vanilla-kernel-srcdir>"
fi

# check valid kernel directory
if test ! -f "$1/MAINTAINERS"; then
	abort invalid kernel source directory.
fi

# check the installation of quilt
if ! which quilt; then
	abort no quilt installed.
fi

# read revision number from file "Revision"
om_kernel_rev=`cat Revision`
if [ "x$om_kernel_rev" = "x" ]; then
	abort Empty OM kernel revision.
fi

# check if patch was applied.
pushd $1
if [ -d patches ]; then
	pushd patches
	if [ "x`svnversion`" != "x${om_kernel_rev}M" ]; then
		abort Mismatch OM kernel patch versions.
	fi
	popd
fi

DM_APPLIED_PATCH=dm_applied.patch

if quilt applied | grep "$DM_APPLIED_PATCH"; then
	echo "DM support for kernel has been applied."
else
	svn co -r$om_kernel_rev \
		http://svn.openmoko.org/trunk/src/target/kernel/patches
	cp -f $SVN_BASEDIR/dm_patches/$DM_APPLIED_PATCH .
	quilt import $DM_APPLIED_PATCH
	rm -f $DM_APPLIED_PATCH
	quilt push -a
	cp -f $SVN_BASEDIR/dot-config .config
fi

# finalize
echo "Now, please check directory $1, which is patched and setup."
