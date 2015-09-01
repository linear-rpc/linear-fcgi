#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([linear-fcgi], [2.0.8], [https://github.com/linear-rpc/linear-fcgi])
AC_CONFIG_AUX_DIR(m4)
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_SRCDIR([app/linear_fcgi.cc])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([src/config.h])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for debug
AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug], [output debug log@<:@default=no@:>@]),
              [debug=$withval], [debug=no])
if test "x${debug}" != "xno"; then
   CFLAGS="$CFLAGS -g -D_LINEAR_FCGI_DEBUG_"
   CXXFLAGS="$CXXFLAGS -g -D_LINEAR_FCGI_DEBUG_"
fi
AM_CONDITIONAL(LINEAR_LOG_DEBUG, test x${debug} != xno)

CFLAGS="$CFLAGS -Wall -Werror -Wno-strict-aliasing"
CXXFLAGS="$CXXFLAGS -Wall -Werror -Wno-strict-aliasing -Wconversion -fthreadsafe-statics"

# Checks for libfcgi
AC_ARG_WITH([fastcgi],
            AC_HELP_STRING([--with-fastcgi], [specify fastcgi library location@<:@default=yes@:>@]),
            [fastcgi=$withval], [fastcgi=yes])
if test "x${fastcgi}" = "xyes"; then
   message="-lfcgi"
   LIBS="$message $LIBS"
elif test "x${fastcgi}" != "xyes"; then
   CFLAGS="$CFLAGS -I$fastcgi/include"
   CXXFLAGS="$CXXFLAGS -I$fastcgi/include"
   message="$fastcgi/lib/libfcgi.a"
   LIBS="$message $LIBS"
fi
AC_MSG_CHECKING([for FCGI_printf in $message])
AC_TRY_LINK([#include <fcgi_stdio.h>], [FCGI_printf("");],
            AC_MSG_RESULT(yes), [AC_MSG_RESULT(no); AC_MSG_ERROR([needs libfcgi])])

# Checks for libpcre
AC_ARG_WITH([pcre],
            AC_HELP_STRING([--with-pcre], [use pcre@<:@default=yes@:>@]),
            [with_pcre=$withval], [with_pcre=yes])
AM_CONDITIONAL([WITH_PCRE], [test "x${with_pcre}" != "xno"])
if test "x${with_pcre}" != "xno"; then
   AC_CHECK_HEADERS([pcre.h])
   AC_CHECK_LIB([pcre], [pcre_compile])
fi

AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([rt], [timer_create])

AC_CONFIG_FILES([
        Makefile
        src/Makefile
        src/uuid/Makefile
        src/uuid/tests/Makefile
        src/uuid/samples/Makefile
        src/tests/Makefile
        app/Makefile
        ])

AC_CONFIG_SUBDIRS([deps/libuv deps/mod_websocket deps/mod_websocket/supplement/config_parser])

AC_OUTPUT
