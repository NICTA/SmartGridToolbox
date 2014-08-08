AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libSgtCore.a

include_HEADERS = LibSgtCore.h

pkginclude_HEADERS = \
	Branch.h \
	Bus.h \
	Common.h \
	CommonBranch.h \
	Gen.h \
	Network.h \
	Parser.h \
	PowerFlow.h \
	Random.h \
	Units.h \
	Zip.h

libSgtCore_a_SOURCES = \
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
	LibSgtCore.h \
	MatpowerParser.h \
	MatpowerParser.cc \
	Network.h \
	Network.cc \
	Parser.h \
	Parser.cc \
	PowerFlow.h \
	PowerFlow.cc \
	Random.h \
	Random.cc \
	RegisterParserPlugins.cc \
	Zip.h \
	Zip.cc \
	Units.h \
	ZipParser.h \
	ZipParser.cc

noinst_PROGRAMS = helloworld

helloworld_SOURCES = helloworld.cc
helloworld_LDADD = libSgtCore.a -lyaml-cpp
