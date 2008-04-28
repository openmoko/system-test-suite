PLUGIN_DIR = /usr/lib/ts
TS_CONF = /etc/ts.conf

CFLAGS += -I. -Isrc/ -DPLUGIN_DIR=\"$(PLUGIN_DIR)\" -DTS_CONF=\"$(TS_CONF)\"

SRC_OBJS = src/ts_attach.o		\
	   src/ts_close.o		\
	   src/ts_config.o		\
	   src/ts_error.o		\
	   src/ts_fd.o			\
	   src/ts_load_module.o		\
	   src/ts_open.o		\
	   src/ts_parse_vars.o		\
	   src/ts_read.o		\
	   src/ts_read_raw.o

PLUGIN_OBJS = plugins/input-raw.o	\
	      plugins/pthres.o		\
 	      plugins/variance.o	\
 	      plugins/dejitter.o	\
 	      plugins/linear.o

OBJS = $(SRC_OBJS) $(PLUGIN_OBJS)

libts.a: $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f libts.a $(OBJS)
