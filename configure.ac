AC_INIT([SmartGridToolbox], [v6.0.3], [dan.gordon@anu.edu.au], [SmartGridToolbox])
AC_CONFIG_SRCDIR([src/run_config.cc])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
LT_INIT([disable-shared])

AC_ARG_ENABLE([power-tools],
              [AS_HELP_STRING([--enable-power-tools], [enable PowerTools specific features @<:@no@:>@])],
              [],
              [enable_power_tools=no])
AC_SUBST([enable_power_tools])
AM_CONDITIONAL([ENABLE_POWER_TOOLS], [test x$enable_power_tools = xyes])
AS_IF([test x$enable_power_tools == xyes],
      [AC_DEFINE([ENABLE_POWER_TOOLS], [], [enable PowerTools support])])

AC_ARG_WITH([gurobi-libs], [AS_HELP_STRING([--with-gurobi-libs=LINK_FLAG], [specify gurobi link flags])])
AC_SUBST([gurobi_libs], [$with_gurobi_libs])

AC_ARG_WITH([bonmin-libs], [AS_HELP_STRING([--with-bonmin-libs=LINK_FLAG], [specify bonmin link flags])])
AC_SUBST([bonmin_libs], [$with_bonmin_libs])

AC_ARG_WITH([klu],
            [AS_HELP_STRING([--without-klu], [do not use the KLU library as a sparse solver (use SuperLU instead) @<:@no@:>@])],
            [],
            [with_klu=yes])
AC_SUBST([with_klu])
AM_CONDITIONAL([WITH_KLU], [test x$with_klu = xyes])
AS_IF([test x$with_klu == xyes],
      [AC_DEFINE([WITH_KLU], [], [use the KLU library as a sparse solver])])

AC_PROG_CXX([clang++ g++])

AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_14([noext])
AX_CHECK_COMPILE_FLAG([-ftemplate-depth=160], [AM_CXXFLAGS="$AM_CXXFLAGS -ftemplate-depth=160"])
AC_SUBST([AM_CXXFLAGS], ["$AM_CXXFLAGS -Wall"])

AX_BOOST_BASE(,, [AC_MSG_ERROR([SmartGridToolbox needs Boost, but it was not found in your system])])
AX_BOOST_DATE_TIME

AC_CHECK_PROG([HAVE_DOT], [dot], ["YES"], ["NO"])
AC_CHECK_PROGS([DOXYGEN], [doxygen])
AC_CHECK_PROGS([MAKEINDEX], [makeindex])
AC_CHECK_PROGS([PDFLATEX], [pdflatex])

if test -z "$DOXYGEN";
    then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

if test -z "$PDFLATEX" || test -z "$MAKEINDEX";
    then AC_MSG_WARN([pdflatex and/or makeindex not found - continuing without pdflatex support])
fi
AM_CONDITIONAL([HAVE_PDFLATEX], [test -n "$PDFLATEX" && test -n "$MAKEINDEX"])

AC_CONFIG_HEADERS([SgtCore/config.h])
AC_CONFIG_FILES([Makefile SgtCore/Makefile SgtSim/Makefile experimental/SgtServer/Makefile src/Makefile tests/Makefile docs/Makefile])
AM_COND_IF(
           [HAVE_DOXYGEN],
           [AC_CONFIG_FILES([docs/Doxyfile])]
           )

AC_SUBST([AM_CPPFLAGS], ["$AM_CPPFLAGS -I.."])

AC_OUTPUT
