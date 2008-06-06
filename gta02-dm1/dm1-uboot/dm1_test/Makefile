include $(TOPDIR)/config.mk

# CFLAGS += -DET_DEBUG -DDEBUG

LIB	= $(obj)libdm1_test.a

AOBJS   =
COBJS-y += cmd_bkbatt.o
COBJS-y += cmd_hdq.o
COBJS-y += cmd_nor_check.o
COBJS-y += cmd_chg.o
COBJS-y += cmd_key911.o
COBJS-y += cmd_sensor.o
COBJS-y += cmd_earphone.o
COBJS-y += cmd_keypower.o
COBJS-y += cmd_usb5v.o

COBJS	:= $(COBJS-y)

SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB) $(AOBJS)

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

