OBJS = descriptors.o	\
       error.o		\
       linux.o		\
       usb.o

libusb.a: $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f libusb.a $(OBJS)
