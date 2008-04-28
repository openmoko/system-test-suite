INCLUDES = $(all_includes) -I$(top_srcdir)/include
AM_CFLAGS = -std=gnu99 -Os
plugindir = $(libdir)/gsmd

gsmd-version.h:
	echo -e '#ifndef GSMD_VERSION' \
	'\n#define GSMD_VERSION "$(VERSION)" \\' > gsmd-version.h
	if test -d .svn; then \
	    echo -e '\t"+'`svnversion`'"' >> gsmd-version.h; \
	else \
	    echo -e '\t/* release version */' >> gsmd-version.h; \
	fi
	echo -e '#endif' >> gsmd-version.h

BUILT_SOURCES = gsmd-version.h
sbin_PROGRAMS = gsmd

gsmd_CFLAGS = -D PLUGINDIR=\"$(plugindir)\"
gsmd_SOURCES = gsmd.c atcmd.c select.c machine.c vendor.c unsolicited.c log.c \
	       usock.c talloc.c timer.c operator_cache.c ext_response.c \
	       sms_cb.c sms_pdu.c vendor_ti.c machine_generic.c
gsmd_LDADD = -ldl
gsmd_LDFLAGS = -Wl,--export-dynamic -all-static

libgsmd_machine_generic_la_SOURCES = machine_generic.c
libgsmd_machine_generic_la_LDFLAGS = -module

libgsmd_vendor_ti_la_SOURCES = vendor_ti.c
libgsmd_vendor_ti_la_LDFLAGS = -module

noinst_HEADERS = gsmd.h
