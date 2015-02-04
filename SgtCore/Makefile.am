AUTOMAKE_OPTIONS = foreign

libincludedir=$(includedir)/SgtCore

lib_LIBRARIES = libSgtCore.a

include_HEADERS = SgtCore.h

libinclude_HEADERS = \
	../config.h \
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
	PowerFlowSolver.h \
	Properties.h \
	Random.h \
	SinglePhaseTransformer.h \
	Stopwatch.h \
	Units.h \
	YamlSupport.h \
	YyTransformer.h \
	Zip.h

if ENABLE_LAYOUT
libinclude_HEADERS += \
	NetworkGraph.h \
	SgtNetworkGraph.h
endif

libSgtCore_a_SOURCES = \
	Branch.h Branch.cc \
	Bus.h Bus.cc \
	BusParserPlugin.h BusParserPlugin.cc \
	Common.h Common.cc \
	CommonBranch.h CommonBranch.cc \
	CommonBranchParserPlugin.h CommonBranchParserPlugin.cc \
	Component.h \
	DgyTransformer.h DgyTransformer.cc \
	DgyTransformerParserPlugin.h DgyTransformerParserPlugin.cc \
	Event.h Event.cc \
	Gen.h Gen.cc \
	GenericBranchParserPlugin.h GenericBranchParserPlugin.cc \
	GenericGenParserPlugin.h GenericGenParserPlugin.cc \
	GenericZipParserPlugin.h GenericZipParserPlugin.cc \
	LibSgtCore.h \
	MatpowerParserPlugin.h MatpowerParserPlugin.cc \
	Network.h Network.cc \
	NetworkParser.h NetworkParser.cc \
	OverheadLine.h OverheadLine.cc \
	OverheadLineParserPlugin.h OverheadLineParserPlugin.cc \
	Parser.h Parser.cc \
	PowerFlow.h PowerFlow.cc \
	PowerFlowModel.h PowerFlowModel.cc \
	PowerFlowNrSolver.h PowerFlowNrSolver.cc \
	PowerFlowSolver.h \
	Properties.h \
	Random.h Random.cc \
	RegisterParserPlugins.cc \
	SparseSolver.h SparseSolver.cc \
	Stopwatch.h \
	SinglePhaseTransformer.h SinglePhaseTransformer.cc \
	SinglePhaseTransformerParserPlugin.h SinglePhaseTransformerParserPlugin.cc \
	Units.h Units.cc \
	YamlSupport.h YamlSupport.cc \
	YyTransformer.h YyTransformer.cc \
	YyTransformerParserPlugin.h YyTransformerParserPlugin.cc \
	Zip.h Zip.cc

if ENABLE_LAYOUT
libSgtCore_a_SOURCES += \
	NetworkGraph.h NetworkGraph.cc \
	SgtNetworkGraph.h SgtNetworkGraph.cc
endif
