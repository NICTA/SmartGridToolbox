AUTOMAKE_OPTIONS = foreign

if ENABLE_POWER_TOOLS_PP
POWER_FLOW_PT_PP_SOLVER_H = PowerFlowPtPpSolver.h
POWER_FLOW_PT_PP_SOLVER_CC = PowerFlowPtPpSolver.cc
L_POWER_TOOLS_PP = -lPowerTools++ -lipopt -lcoinhsl -lgurobi65 -lgurobi_c++
else
POWER_FLOW_PT_PP_SOLVER_H =
POWER_FLOW_PT_PP_SOLVER_CC =
L_POWER_TOOLS_PP =
endif

if WITH_KLU
KLU_SOLVER_H = KluSolver.h
KLU_SOLVER_CC = KluSolver.cc
L_KLU_SOLVER = -lklu -lamd -lcolamd -lbtf
else
KLU_SOLVER_H =
KLU_SOLVER_CC =
L_KLU_SOLVER =
endif

libincludedir = $(includedir)/SgtCore

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
	json.hpp \
	Network.h \
	NetworkParser.h \
	OverheadLine.h \
	Parser.h \
	PowerFlow.h \
	PowerFlowFdSolver.h \
	PowerFlowModel.h \
	PowerFlowNrPolSolver.h \
	PowerFlowNrRectSolver.h \
	$(POWER_FLOW_PT_PP_SOLVER_H) \
	PowerFlowSolver.h \
	Properties.h \
	Random.h \
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
	json.hpp \
	$(KLU_SOLVER_H) $(KLU_SOLVER_CC) \
	MatpowerParserPlugin.h MatpowerParserPlugin.cc \
	Network.h Network.cc \
	NetworkParser.h NetworkParser.cc \
	OverheadLine.h OverheadLine.cc \
	OverheadLineParserPlugin.h OverheadLineParserPlugin.cc \
	Parser.h Parser.cc \
	PowerFlow.h PowerFlow.cc \
	PowerFlowFdSolver.h PowerFlowFdSolver.cc \
	PowerFlowModel.h PowerFlowModel.cc \
	PowerFlowNrPolSolver.h PowerFlowNrPolSolver.cc \
	PowerFlowNrRectSolver.h PowerFlowNrRectSolver.cc \
	$(POWER_FLOW_PT_PP_SOLVER_H) $(POWER_FLOW_PT_PP_SOLVER_CC) \
	PowerFlowSolver.h PowerFlowSolver.cc \
	PowerFlowSolverParserPlugin.h PowerFlowSolverParserPlugin.cc \
	Properties.h \
	Random.h Random.cc \
	RegisterParserPlugins.cc \
	SparseHelper.h \
	SgtCore.h \
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
	$(L_KLU_SOLVER) \
	$(L_POWER_TOOLS_PP)

noinst_PROGRAMS = helloworld
helloworld_SOURCES = helloworld.cc
helloworld_LDADD = -lSgtCore
