AUTOMAKE_OPTIONS = foreign

AM_LDFLAGS = \
	-framework Foundation \
	-framework PowerTools \
	-lipopt \
	-lcoinhsl

lib_LIBRARIES = libSgtPowerTools.a

include_HEADERS = SgtPowerTools.h

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
