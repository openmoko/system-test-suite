AUTOMAKE_OPTIONS = foreign dist-bzip2 1.6

SUBDIRS = include src
#LINKOPTS = -lusb

AM_CFLAGS = -std=gnu99 -Os

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

pcdata_DATA= libgsmd.pc
pcdatadir= $(libdir)/pkgconfig

EXTRA_DIST= $(pcdata_DATA)
