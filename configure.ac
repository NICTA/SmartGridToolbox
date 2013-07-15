AC_INIT([SmartGridToolbox], [0.1], [dan.gordon@nicta.com.au])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN], 
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_LANG([C++])
m4_include([./m4_ax_cxx_compile_stdcxx_11.m4])
AX_CXX_COMPILE_STDCXX_11
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile docs/Makefile])
AC_OUTPUT
