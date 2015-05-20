AC_INIT([SmartGridToolbox], [v2.0.0b1], [dan.gordon@nicta.com.au], [SmartGridToolbox])
AC_CONFIG_SRCDIR([src/run_config.cc])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_ARG_ENABLE([power_tools_pp],
              [AS_HELP_STRING([--enable-power_tools_pp], [enable PowerTools++ specific features @<:@no@:>@])],
              [],
              [enable_power_tools_pp=no])
AC_SUBST([enable_power_tools_pp])
AM_CONDITIONAL([ENABLE_POWER_TOOLS_PP], [test x$enable_power_tools_pp = xyes])
AS_IF([test x$enable_power_tools_pp == xyes],
      [AC_DEFINE([ENABLE_POWER_TOOLS_PP], [], [enable PowerTools++ support])])

AC_ARG_WITH([klu],
              [AS_HELP_STRING([--with-klu], [use the KLU library as a sparse solver @<:@no@:>@])],
              [],
              [with_klu=no])
AC_SUBST([with_klu])
AM_CONDITIONAL([WITH_KLU], [test x$with_klu = xyes])
AS_IF([test x$with_klu == xyes],
      [AC_DEFINE([WITH_KLU], [], [use the KLU library as a sparse solver])])

AC_PROG_CC([clang gcc])
AC_PROG_CXX([clang++ g++])
AC_PROG_LIBTOOL
LT_INIT
AC_PROG_OBJCXX([clang]) # Unfortunately it is an autoconf error to make this macro conditional.

AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext])
AX_CHECK_COMPILE_FLAG([-ftemplate-depth=160], [AM_CXXFLAGS="$AM_CXXFLAGS -ftemplate-depth=160"])
AC_SUBST([AM_CXXFLAGS], ["$AM_CXXFLAGS -Wall"])

# See above comments for AC_PROG_OBJCXX
AC_LANG([Objective C++])
AX_CHECK_COMPILE_FLAG([-std=c++11], [AC_SUBST([AM_OBJCXXFLAGS], ["-std=c++11"])])

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

AC_CONFIG_HEADERS([SgtCore/config.h])
AC_CONFIG_FILES([Makefile SgtCore/Makefile SgtSim/Makefile src/Makefile tests/Makefile docs/Makefile])
AM_COND_IF(
           [HAVE_DOXYGEN],
           [AC_CONFIG_FILES([docs/Doxyfile])]
           )

AC_SUBST([AM_CPPFLAGS], ["$AM_CPPFLAGS -I.."])

AC_OUTPUT
