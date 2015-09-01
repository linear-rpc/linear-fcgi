TESTS = \
	urlendecode_test \
	linear_fcgi_queue_test \
	linear_fcgi_env_test \
	linear_fcgi_queries_test \
	linear_fcgi_response_test \
	linear_fcgi_request_test

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I/opt/local/include

LDADD = \
	../liblinear_fcgi.a

AM_LDFLAGS = \
	-L/opt/local/lib \
	-lgtest

check_PROGRAMS = \
	urlendecode_test \
	linear_fcgi_queue_test \
	linear_fcgi_env_test \
	linear_fcgi_queries_test \
	linear_fcgi_response_test \
	linear_fcgi_request_test

urlendecode_test_SOURCES = \
	urlendecode_test.cc

linear_fcgi_queue_test_SOURCES = \
	linear_fcgi_queue_test.cc

linear_fcgi_env_test_SOURCES = \
	linear_fcgi_env_test.cc

linear_fcgi_queries_test_SOURCES = \
	linear_fcgi_queries_test.cc

linear_fcgi_response_test_SOURCES = \
	linear_fcgi_response_test.cc

linear_fcgi_request_test_SOURCES = \
	linear_fcgi_request_test.cc

clean-local:
	${RM} -f *~
