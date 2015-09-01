SUBDIRS = uuid . tests

MOD_WEBSOCKET_DIR = $(top_srcdir)/deps/mod_websocket
LIBUV_DIR = $(top_srcdir)/deps/libuv

noinst_LIBRARIES = liblinear_fcgi.a

liblinear_fcgi_a_CPPFLAGS = \
	-I$(MOD_WEBSOCKET_DIR)/src \
	-I$(MOD_WEBSOCKET_DIR)/supplement/config_parser/src \
	-I$(LIBUV_DIR)/include \
	-I.

liblinear_fcgi_a_SOURCES = \
	urlendecode.c \
	linear_fcgi_env.cc \
	linear_fcgi_queries.cc \
	linear_fcgi_response.cc \
	linear_fcgi_request.cc \
	linear_fcgi_loop.cc \
	linear_fcgi_log.cc \
	linear_fcgi_client.cc \
	linear_fcgi_client_storage.cc \
	linear_fcgi_server.cc

clean-local:
	${RM} -f *~
