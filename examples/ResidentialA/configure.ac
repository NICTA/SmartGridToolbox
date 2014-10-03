AC_INIT([residential_a], [v1.0], [dan.gordon@nicta.com.au])
AC_CONFIG_SRCDIR([residential_a.cc])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

m4_include([./m4_ax_cxx_compile_stdcxx_11.m4])
m4_include([./m4_ax_check_compile_flag.m4])

AC_PROG_CXX([clang++ g++])
AC_PROG_RANLIB

AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext])
AX_CHECK_COMPILE_FLAG([-ftemplate-depth=160], [AM_CXXFLAGS="$AM_CXXFLAGS -ftemplate-depth=160"])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
