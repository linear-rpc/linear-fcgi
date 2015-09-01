LIBUV_DIR = $(top_srcdir)/deps/libuv
MOD_WEBSOCKET_DIR = $(top_srcdir)/deps/mod_websocket

bin_PROGRAMS = linear_fcgi

linear_fcgi_CPPFLAGS = \
	-I$(MOD_WEBSOCKET_DIR)/src \
	-I$(MOD_WEBSOCKET_DIR)/supplement/config_parser/src \
	-I$(LIBUV_DIR)/include \
	-I$(top_srcdir)/src

linear_fcgi_SOURCES = linear_fcgi.cc

linear_fcgi_LDADD = \
	$(top_srcdir)/src/liblinear_fcgi.a \
	$(top_srcdir)/src/uuid/libuuid.a \
	$(MOD_WEBSOCKET_DIR)/src/.libs/libmod_websocket.a \
	$(MOD_WEBSOCKET_DIR)/supplement/config_parser/src/.libs/libmod_websocket_config_parser.a \
	$(LIBUV_DIR)/.libs/libuv.a

clean-local:
	${RM} -f *~
