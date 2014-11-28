AC_INIT([SmartGridToolbox], [v2.0.0], [dan.gordon@nicta.com.au], [SmartGridToolbox])
AC_CONFIG_SRCDIR([src/run_config.cc])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
LT_INIT

#m4_include([m4/m4_ax_cxx_compile_stdcxx_11.m4])
#m4_include([m4/m4_ax_check_compile_flag.m4])

AC_PROG_CC
AC_PROG_CXX([clang++ g++])
AC_PROG_OBJCXX([clang])

AC_LANG([Objective C++])
AX_CHECK_COMPILE_FLAG(-std=c++11, [AC_SUBST(AM_OBJCXXFLAGS, ["-std=c++11"])])

AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext])
AX_CHECK_COMPILE_FLAG([-ftemplate-depth=160], [AM_CXXFLAGS="$AM_CXXFLAGS -ftemplate-depth=160"])
AC_SUBST([AM_CXXFLAGS], ["$AM_CXXFLAGS -Wall"])

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
AC_CONFIG_FILES([Makefile SgtCore/Makefile SgtSim/Makefile src/Makefile tests/Makefile docs/Makefile])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

AC_OUTPUT
