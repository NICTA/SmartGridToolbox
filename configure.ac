AC_INIT([SmartGridToolbox], [0.1], [dan.gordon@nicta.com.au])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

m4_include([./m4_ax_cxx_compile_stdcxx_11.m4])
m4_include([./m4_ax_prog_doxygen.m4])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXX
AC_PROG_RANLIB
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11

DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(SmartGridToolbox, Doxyfile)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile docs/Makefile \
                 docs/Doxyfile])
AC_OUTPUT
