#export PATH=/usr/local/openmoko-pbe/usr/bin/:$PATH
#TOOLCHAIN_PREFIX = /usr/local/openmoko-pbe/usr/bin/arm-linux-uclibc-# pbe compiler is error
#TOOLCHAIN_PREFIX = arm-linux-uclibc-//for PBE
TOOLCHAIN_PREFIX = arm-angstrom-linux-gnueabi-

CC = $(TOOLCHAIN_PREFIX)gcc
AR = $(TOOLCHAIN_PREFIX)ar
STRIP = $(TOOLCHAIN_PREFIX)strip
RANLIB = $(TOOLCHAIN_PREFIX)ranlib

CFLAGS = -Wall -Os

export CC AR RANLIB STRIP CFLAGS

BINS = main gsmd
OBJS = \
       main.o \
       audio.o \
       backlight.o \
       bluetooth.o \
       gps.o \
       graphics.o \
       gsm.o \
       key.o \
       lcd.o \
       batt.o \
       led.o \
       norflash.o \
       rtc.o \
       sdcard.o \
       sensor.o \
       touchpanel.o \
       vibrator.o \
       wireless.o

LIBS = \
	libts/libts.a \
	libusb/libusb.a \
	libglamo/libglamo.a

all: $(BINS)

main: $(LIBS) $(OBJS)
	$(CC) -static $(CFLAGS) -o $@ $(OBJS) $(LIBS) 
#	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) 
	$(STRIP) -x $@

gsmd:
	cd gsm; ./autogen.sh; ./crossconf; make; cp src/gsmd/gsmd ../; \
	cp src/util/libgsmd-tool ../;

$(LIBS):
	@$(MAKE) -C $(dir $@)

clean:
	rm -f $(BINS) $(OBJS)
	@for i in $(dir $(LIBS)); do \
		$(MAKE) -C $$i clean; \
	done
