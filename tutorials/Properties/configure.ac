AC_INIT([tutorials], [v1.0.0], [dan.gordon@nicta.com.au], [Template])
AC_CONFIG_SRCDIR([network.cc])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX([clang++ g++])
AC_PROG_LIBTOOL

AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext])
AX_CHECK_COMPILE_FLAG([-ftemplate-depth=160], [AM_CXXFLAGS="$AM_CXXFLAGS -ftemplate-depth=160"])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
