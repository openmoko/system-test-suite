AC_PREREQ(2.53)
AC_INIT(dm2, 1.30, anthony_chang@openmoko.com)
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([src/dm2.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AC_STDC_HEADERS

AC_ARG_ENABLE(x11, [AC_HELP_STRING([--disable-x11],
	                         [disable X11 backend])],,
	      [enable_x11=yes])

AC_ARG_ENABLE(fb, [AC_HELP_STRING([--disable-fb],
	      			  [disable linux framebuffer backend])],,
	      [enable_fb=yes])

if test "x$enable_x11" = "xyes"; then
	PKG_CHECK_MODULES(X11, x11,,[enable_x11=no])
	if test "x$enable_x11" = "xyes"; then
		AC_DEFINE(HAVE_X11, 1, [Compile with X11 support])
	fi
fi
AM_CONDITIONAL(HAVE_X11, test "$enable_x11" = "yes")

if test "x$enable_fb" = "xyes"; then
	PKG_CHECK_MODULES(FB, tslib-1.0,,[enable_fb=no])
	if test "x$enable_fb" = "xyes"; then
		AC_DEFINE(HAVE_FB, 1, [Compile with linux framebuffer support])
	fi
fi
AM_CONDITIONAL(HAVE_FB, test "$enable_fb" = "yes")

# base deps

dnl Adds -fvisibility=hidden to CFLAGS if running with gcc 4 or greater.
AC_MSG_CHECKING([whether the compiler supports the visibility arg])
dnl Check for gcc4 or greater
AC_TRY_COMPILE([
void
#if defined(__GNUC__) && (__GNUC__ >= 4)
foo () {};                                                                                         
#endif
], [], [
  has_visibility=yes
  CFLAGS="$CFLAGS -fvisibility=hidden"
], [
  has_visibility=no
]
)
AC_MSG_RESULT($has_visibility)

AC_OUTPUT([
	Makefile
	src/Makefile
	src/oltk/Makefile
	data/Makefile
])

echo
echo dm2..... ........................ : Version $VERSION
echo
echo prefix........................... : $prefix
echo Linux framebuffer backend ....... : $enable_fb
echo X11 simulation backend .......... : $enable_x11
