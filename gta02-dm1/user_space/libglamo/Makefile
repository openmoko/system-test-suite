RANLIB ?= ranlib

OBJS = os.o hw.o dma.o blit.o
HEADERS = dma.h glamo.h glamo-regs.h hw.h blit.h

all: libglamo.a

$(OBJS): $(HEADERS)

libglamo.a: $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f libglamo.a $(OBJS)
