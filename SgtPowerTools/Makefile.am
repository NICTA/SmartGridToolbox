AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -F . -I /Library/gurobi563/mac64/include
AM_LDFLAGS = -F . -framework Foundation -framework PowerTools

lib_LIBRARIES = libSgtPowerTools.a

include_HEADERS = LibSgtPowerTools.h

pkginclude_HEADERS =

libSgtPowerTools_a_SOURCES = \
	SgtPowerTools.h \
	SgtPowerTools.cc \
	PowerToolsCpp.h \
	PowerToolsCpp.mm

noinst_PROGRAMS = helloworld

helloworld_SOURCES = helloworld.cc
helloworld_CPPFLAGS = -F .
helloworld_LDADD = libSgtPowerTools.a -lSgtCore -lyaml-cpp
