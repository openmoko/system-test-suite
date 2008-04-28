SUBDIRS = oltk

bin_PROGRAMS = dm2.bin

noinst_HEADERS = \
		 dm2_Network.h \
		 nmea.h \
		 gps.h

dm2_bin_SOURCES = \
	dm2.c \
	dm2_Network.c \
	dm2_Network.h \
	nmea.c \
	nmea.h \
	gps.h \
	gps.c \
	tests-version.c \
	tests-rtc.c \
	tests-pmu.c \
	tests-mac.c \
	tests-vibrator.c \
	tests-leds.c \
	tests-audio.c \
	tests-lcm.c \
	tests-motion.c \
	tests-cn.c \
	tests-keys.c \
	tests-gps.c \
	tests-bluetooth.c \
	tests-wifi.c \
	tests-gsm.c \
	tests-logs.c

dm2_bin_LDADD = \
		$(top_builddir)/src/oltk/liboltk.la $(FB_LIBS) 

