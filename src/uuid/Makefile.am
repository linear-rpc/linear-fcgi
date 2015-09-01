SUBDIRS = . tests samples

noinst_LIBRARIES = libuuid.a

libuuid_a_CFLAGS = \
	-I.

libuuid_a_SOURCES = \
	uuid.c \
	_uuid_impl.c \
	_uuid_node.c \
	_uuid_time.c \
	_uuid_clock.c

clean-local:
	${RM} -f *~
