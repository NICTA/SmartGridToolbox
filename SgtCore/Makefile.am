AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libSgtCore.a

include_HEADERS = LibSgtCore.h

pkginclude_HEADERS = \
	Branch.h \
	Bus.h \
	Common.h \
	CommonBranch.h \
	Component.h \
	DgyTransformer.h \
	Event.h \
	Gen.h \
	Network.h \
	NetworkParser.h \
	OverheadLine.h \
	Parser.h \
	PowerFlow.h \
	Random.h \
	SinglePhaseTransformer.h \
	Units.h \
	YyTransformer.h \
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
	Component.h \
	DgyTransformer.h \
	DgyTransformer.cc \
	DgyTransformerParser.h \
	DgyTransformerParser.cc \
	Event.h \
	Event.cc \
	Gen.h \
	Gen.cc \
	GenericBranchParser.h \
	GenericBranchParser.cc \
	GenericGenParser.h \
	GenericGenParser.cc \
	GenericZipParser.h \
	GenericZipParser.cc \
	LibSgtCore.h \
	MatpowerParser.h \
	MatpowerParser.cc \
	Network.h \
	Network.cc \
	NetworkParser.h \
	NetworkParser.cc \
	OverheadLine.h \
	OverheadLine.cc \
	OverheadLineParser.h \
	OverheadLineParser.cc \
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
	SinglePhaseTransformer.h \
	SinglePhaseTransformer.cc \
	SinglePhaseTransformerParser.h \
	SinglePhaseTransformerParser.cc \
	Units.h \
	YyTransformer.h \
	YyTransformer.cc \
	YyTransformerParser.h \
	YyTransformerParser.cc \
	Zip.h \
	Zip.cc

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
