TESTS = \
	_uuid_time_test \
	_uuid_clock_test \
	_uuid_node_test.sh \
	_uuid_impl_test \
	uuid_test

AM_CPPFLAGS = \
	-I../ \
	-I/opt/local/include

AM_LDFLAGS = \
	-L/opt/local/lib

LDADD = \
	../libuuid.a \
	-lgtest

check_PROGRAMS = \
	_uuid_time_test \
	_uuid_clock_test \
	_uuid_node_test \
	_uuid_impl_test \
	uuid_test

_uuid_time_test_SOURCES = \
	_uuid_time_test.cc

_uuid_clock_test_SOURCES = \
	_uuid_clock_test.cc

_uuid_node_test_SOURCES = \
	_uuid_node_test.cc

_uuid_impl_test_SOURCES = \
	_uuid_impl_test.cc

uuid_test_SOURCES = \
	uuid_test.cc

clean-local:
	${RM} -f *~
