GR_BACKENDS =

if HAVE_X11
GR_BACKENDS += gr_x11.c
endif

if HAVE_FB
GR_BACKENDS += gr_fb.c
endif

noinst_LTLIBRARIES = liboltk.la

AM_CPPFLAGS = $(X11_CFLAGS) $(FB_CFLAGS)

noinst_HEADERS = gr.h gr_impl.h oltk.h oltk_utils.h font.h

liboltk_la_SOURCES = \
	gr.c \
	$(GR_BACKENDS) \
	oltk.c \
	font_8x13.c

liboltk_la_LIBADD = $(X11_LIBS) $(FB_LIBS)
