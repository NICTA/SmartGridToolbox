AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libPowerFlow.a

include_HEADERS = LibPowerFlow.h

pkginclude_HEADERS = \
	Branch.h \
	Bus.h \
	Common.h \
	CommonBranch.h \
	Gen.h \
	Network.h \
	Parser.h \
	PowerFlow.h \
	Zip.h

libPowerFlow_a_SOURCES = \
	Branch.h \
	Branch.cc \
	Bus.h \
	Bus.cc \
	BusParser.h \
	BusParser.cc \
	Common.h \
	Common.cc \
	CommonBranch.h \
	CommonBranch.cc \
	CommonBranchParser.h \
	CommonBranchParser.cc \
	Gen.h \
	Gen.cc \
	GenParser.h \
	GenParser.cc \
	LibPowerFlow.h \
	MatpowerParser.h \
	MatpowerParser.cc \
	Network.h \
	Network.cc \
	Parser.h \
	Parser.cc \
	PowerFlow.h \
	PowerFlow.cc \
	RegisterParserPlugins.cc \
	Zip.h \
	Zip.cc \
	ZipParser.h \
	ZipParser.cc

noinst_PROGRAMS = helloworld

helloworld_SOURCES = helloworld.cc
helloworld_LDADD = libPowerFlow.a -lyaml-cpp
