AC_INIT([SmartGridToolbox], [v2.0.0], [dan.gordon@nicta.com.au], [SmartGridToolbox])
AC_CONFIG_SRCDIR([src/run_config.cc])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_ARG_ENABLE([layout],
              [AS_HELP_STRING([--enable-layout], [enable layout support @<:@no@:>@])],
              [],
              [enable_layout=no])
AC_SUBST([enable_layout])
AM_CONDITIONAL([ENABLE_LAYOUT], [test x$enable_layout = xyes])
AS_IF([test x$enable_layout == xyes],
      [AC_DEFINE([ENABLE_LAYOUT], [], [enable layout support])])

AC_ARG_ENABLE([power_tools],
              [AS_HELP_STRING([--enable-power_tools], [enable PowerTools specific features @<:@no@:>@])],
              [],
              [enable_power_tools=no])
AC_SUBST([enable_power_tools])
AM_CONDITIONAL([ENABLE_POWER_TOOLS], [test x$enable_power_tools = xyes])
AS_IF([test x$enable_power_tools == xyes],
      [AC_DEFINE([ENABLE_POWER_TOOLS], [], [enable PowerTools support])])

AC_ARG_ENABLE([power_tools_pp],
              [AS_HELP_STRING([--enable-power_tools_pp], [enable PowerTools++ specific features @<:@no@:>@])],
              [],
              [enable_power_tools_pp=no])
AC_SUBST([enable_power_tools_pp])
AM_CONDITIONAL([ENABLE_POWER_TOOLS_PP], [test x$enable_power_tools_pp = xyes])
AS_IF([test x$enable_power_tools_pp == xyes],
      [AC_DEFINE([ENABLE_POWER_TOOLS_PP], [], [enable PowerTools++ support])])

AC_PROG_CC
AC_PROG_CXX([clang++ g++])
AC_PROG_RANLIB
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
