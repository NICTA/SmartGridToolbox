AC_INIT([SmartGridToolbox], [v1.0.2], [dan.gordon@nicta.com.au], [SmartGridToolbox])
AC_CONFIG_SRCDIR([src/run_config.cc])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

m4_include([./m4_ax_cxx_compile_stdcxx_11.m4])
m4_include([./m4_ax_check_compile_flag.m4])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXX
AC_PROG_RANLIB
AC_LANG([C++])

AX_CXX_COMPILE_STDCXX_11
AX_CHECK_COMPILE_FLAG([-ftemplate-depth=160], [AM_CXXFLAGS="$AM_CXXFLAGS -ftemplate-depth=160"])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
AC_CHECK_PROGS([PDFLATEX], [pdflatex])
AC_CHECK_PROGS([MAKEINDEX], [makeindex])

if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

if test -z "$PDFLATEX" || test -z "$MAKEINDEX";
   then AC_MSG_WARN([pdflatex and/or makeindex not found - continuing without pdflatex support])
fi
AM_CONDITIONAL([HAVE_PDFLATEX], [test -n "$PDFLATEX" && test -n "$MAKEINDEX"])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile SmartGridToolbox/Makefile src/Makefile tests/Makefile docs/Makefile])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

AC_OUTPUT
