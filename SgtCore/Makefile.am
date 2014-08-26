AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libSgtCore.a

include_HEADERS = LibSgtCore.h

pkginclude_HEADERS = \
	BranchAbc.h \
	Bus.h \
	Common.h \
	CommonBranch.h \
	Component.h \
	Gen.h \
	GenAbc.h \
	GenericBranch.h \
	Model.h \
	Network.h \
	Parser.h \
	PowerFlow.h \
	Random.h \
	Units.h \
	ZipAbc.h \
	Zip.h

libSgtCore_a_SOURCES = \
	BranchAbc.h \
	BranchAbc.cc \
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
	Component.h \
	Component.cc \
	DgyTransformer.h \
	DgyTransformer.cc \
	DgyTransformerParser.h \
	DgyTransformerParser.cc \
	Gen.h \
	Gen.cc \
	GenAbc.h \
	GenAbc.cc \
	GenericBranch.h \
	GenericBranchParser.h \
	GenericBranchParser.cc \
	GenParser.h \
	GenParser.cc \
	LibSgtCore.h \
	MatpowerParser.h \
	MatpowerParser.cc \
	Model.h \
	Model.cc \
	Network.h \
	Network.cc \
	Parser.h \
	Parser.cc \
	PowerFlow.h \
	PowerFlow.cc \
	PowerFlowNr.h \
	PowerFlowNr.cc \
	Random.h \
	Random.cc \
	RegisterParserPlugins.cc \
	SparseSolver.h \
	SparseSolver.cc \
	Stopwatch.h \
	RegisterParserPlugins.cc \
	Units.h \
	ZipAbc.h \
	ZipAbc.cc \
	Zip.h \
	Zip.cc \
	ZipParser.h \
	ZipParser.cc

noinst_PROGRAMS = helloworld

helloworld_SOURCES = helloworld.cc
helloworld_LDADD = \
	libSgtCore.a \
	-lyaml-cpp \
	-lboost_date_time-mt \
	-lblas \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf
