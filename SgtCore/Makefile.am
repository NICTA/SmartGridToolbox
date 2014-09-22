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
	BusParserPlugin.h \
	BusParserPlugin.cc \
	Common.h \
	Common.cc \
	CommonBranch.h \
	CommonBranch.cc \
	CommonBranchParserPlugin.h \
	CommonBranchParserPlugin.cc \
	Component.h \
	DgyTransformer.h \
	DgyTransformer.cc \
	DgyTransformerParserPlugin.h \
	DgyTransformerParserPlugin.cc \
	Event.h \
	Event.cc \
	Gen.h \
	Gen.cc \
	GenericBranchParserPlugin.h \
	GenericBranchParserPlugin.cc \
	GenericGenParserPlugin.h \
	GenericGenParserPlugin.cc \
	GenericZipParserPlugin.h \
	GenericZipParserPlugin.cc \
	LibSgtCore.h \
	MatpowerParserPlugin.h \
	MatpowerParserPlugin.cc \
	Network.h \
	Network.cc \
	NetworkParser.h \
	NetworkParser.cc \
	OverheadLine.h \
	OverheadLine.cc \
	OverheadLineParserPlugin.h \
	OverheadLineParserPlugin.cc \
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
	SinglePhaseTransformerParserPlugin.h \
	SinglePhaseTransformerParserPlugin.cc \
	Units.h \
	YyTransformer.h \
	YyTransformer.cc \
	YyTransformerParserPlugin.h \
	YyTransformerParserPlugin.cc \
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
