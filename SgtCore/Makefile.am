AUTOMAKE_OPTIONS = foreign

if ENABLE_POWER_TOOLS
SGT_POWER_TOOLS_H = SgtPowerTools.h
SGT_POWER_TOOLS_MM = SgtPowerTools.mm
POWER_FLOW_PT_SOLVER_H = PowerFlowPtSolver.h
L_POWER_TOOLS = -lPowerTools -lipopt -lcoinhsl
else
SGT_POWER_TOOLS_H =
SGT_POWER_TOOLS_MM =
POWER_FLOW_PT_SOLVER_H =
L_POWER_TOOLS =
endif

if ENABLE_POWER_TOOLS_PP
POWER_FLOW_PT_PP_SOLVER_H = PowerFlowPtPpSolver.h
POWER_FLOW_PT_PP_SOLVER_CC = PowerFlowPtPpSolver.cc
L_POWER_TOOLS_PP = -lPowerTools++ -lipopt -lcoinhsl
else
POWER_FLOW_PT_PP_SOLVER_H =
POWER_FLOW_PT_PP_SOLVER_CC =
L_POWER_TOOLS_PP =
L_IPOPT =
L_COIN_HSL =
endif

libincludedir=$(includedir)/SgtCore

lib_LTLIBRARIES = libSgtCore.la

include_HEADERS = SgtCore.h

libinclude_HEADERS = \
	Branch.h \
	Bus.h \
	Common.h \
	CommonBranch.h \
	Component.h \
	config.h \
	DgyTransformer.h \
	Event.h \
	Gen.h \
	Network.h \
	NetworkParser.h \
	OverheadLine.h \
	Parser.h \
	PowerFlow.h \
	PowerFlowModel.h \
	PowerFlowSolver.h \
	PowerFlowNrSolver.h \
	$(POWER_FLOW_PT_SOLVER_H) \
	$(POWER_FLOW_PT_PP_SOLVER_H) \
	Properties.h \
	Random.h \
	$(SGT_POWER_TOOLS_H) \
	SinglePhaseTransformer.h \
	Spline.h \
	Stopwatch.h \
	UndergroundLine.h \
	Units.h \
	YamlSupport.h \
	YyTransformer.h \
	Zip.h

libSgtCore_la_SOURCES = \
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
	$(POWER_FLOW_PT_SOLVER_H) \
	$(POWER_FLOW_PT_PP_SOLVER_H) $(POWER_FLOW_PT_PP_SOLVER_CC)\
	PowerFlowSolver.h PowerFlowSolver.cc \
	PowerFlowSolverParserPlugin.h PowerFlowSolverParserPlugin.cc \
	Properties.h \
	Random.h Random.cc \
	RegisterParserPlugins.cc \
	$(SGT_POWER_TOOLS_H) $(SGT_POWER_TOOLS_MM) \
	SparseSolver.h SparseSolver.cc \
	Spline.h \
	Stopwatch.h \
	SinglePhaseTransformer.h SinglePhaseTransformer.cc \
	SinglePhaseTransformerParserPlugin.h SinglePhaseTransformerParserPlugin.cc \
	UndergroundLine.h UndergroundLine.cc \
	UndergroundLineParserPlugin.h UndergroundLineParserPlugin.cc \
	Units.h Units.cc \
	YamlSupport.h YamlSupport.cc \
	YyTransformer.h YyTransformer.cc \
	YyTransformerParserPlugin.h YyTransformerParserPlugin.cc \
	Zip.h Zip.cc

libSgtCore_la_LIBADD = \
	-larmadillo \
	-lboost_date_time \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf \
	$(L_POWER_TOOLS) \
	$(L_POWER_TOOLS_PP)

noinst_PROGRAMS = helloworld
helloworld_SOURCES = helloworld.cc
