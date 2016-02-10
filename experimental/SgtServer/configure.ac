AC_INIT([SgtServer], [0.1], [dan.gordon@nicta.com.au])
AC_CONFIG_SRCDIR([sgt_server.cc])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_CXX([clang++ g++])
AC_PROG_RANLIB
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext])
AX_CHECK_COMPILE_FLAG([-ftemplate-depth=160], [AM_CXXFLAGS="$AM_CXXFLAGS -ftemplate-depth=160"])
AC_SUBST([AM_CXXFLAGS], ["$AM_CXXFLAGS -Wall"])

AX_BOOST_BASE(,, [AC_MSG_ERROR([SmartGridToolbox needs Boost, but it was not found in your system])])
AX_BOOST_CHRONO
AX_BOOST_FILESYSTEM
AX_BOOST_REGEX
AX_BOOST_SYSTEM
AX_BOOST_THREAD

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
