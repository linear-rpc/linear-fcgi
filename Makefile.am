SUBDIRS = . src app

all-local:
	(cd deps/libuv; $(MAKE))
	(cd deps/mod_websocket; $(MAKE))
	(cd deps/mod_websocket/supplement/config_parser; $(MAKE))

clean-local:
	(cd deps/libuv; $(MAKE) clean)
	(cd deps/mod_websocket; $(MAKE) clean)
	(cd deps/mod_websocket/supplement/config_parser; $(MAKE) clean)
