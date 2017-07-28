AUTOMAKE_OPTIONS = foreign

if ENABLE_POWER_TOOLS
POWER_FLOW_PT_SOLVER_H = PowerFlowPtSolver.h
POWER_TOOLS_SUPPORT_H = PowerToolsSupport.h
POWER_FLOW_PT_SOLVER_CC = PowerFlowPtSolver.cc
POWER_TOOLS_SUPPORT_CC = PowerToolsSupport.cc
L_POWER_TOOLS = -lPowerTools++ -lipopt -lcoinhsl $(gurobi_libs) $(bonmin_libs)
else
POWER_FLOW_PT_SOLVER_H =
POWER_TOOLS_SUPPORT_H =
POWER_FLOW_PT_SOLVER_CC =
POWER_TOOLS_SUPPORT_CC =
L_POWER_TOOLS =
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
	ComponentCollection.h \
	config.h \
	DdTransformer.h \
	DgyTransformer.h \
	Event.h \
	Gen.h \
	json.h \
	Network.h \
	NetworkParser.h \
	OverheadLine.h \
	Parser.h \
	PowerFlow.h \
	PowerFlowModel.h \
	PowerFlowNrRectSolver.h \
	$(POWER_FLOW_PT_SOLVER_H) \
	PowerFlowSolver.h \
	$(POWER_TOOLS_SUPPORT_H) \
	Properties.h \
	Random.h \
	SinglePhaseTransformer.h \
	Spline.h \
	Stopwatch.h \
	Transformer.h \
	UndergroundLine.h \
	Units.h \
	VvTransformer.h \
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
	ComponentCollection.h \
	DdTransformer.h DdTransformer.cc \
	DdTransformerParserPlugin.h DdTransformerParserPlugin.cc \
	DgyTransformer.h DgyTransformer.cc \
	DgyTransformerParserPlugin.h DgyTransformerParserPlugin.cc \
	Event.h Event.cc \
	Gen.h Gen.cc \
	GenericBranchParserPlugin.h GenericBranchParserPlugin.cc \
	GenParserPlugin.h GenParserPlugin.cc \
	json.h \
	$(KLU_SOLVER_H) $(KLU_SOLVER_CC) \
	MatpowerParserPlugin.h MatpowerParserPlugin.cc \
	Network.h Network.cc \
	NetworkParser.h NetworkParser.cc \
	OverheadLine.h OverheadLine.cc \
	OverheadLineParserPlugin.h OverheadLineParserPlugin.cc \
	Parser.h Parser.cc \
	PowerFlow.h PowerFlow.cc \
	PowerFlowModel.h PowerFlowModel.cc \
	PowerFlowNrRectSolver.h PowerFlowNrRectSolver.cc \
	$(POWER_FLOW_PT_SOLVER_H) $(POWER_FLOW_PT_SOLVER_CC) \
	PowerFlowSolver.h \
	PowerFlowSolverParserPlugin.h PowerFlowSolverParserPlugin.cc \
	$(POWER_TOOLS_SUPPORT_H) $(POWER_TOOLS_SUPPORT_CC) \
	Properties.h \
	Random.h Random.cc \
	RegisterParserPlugins.cc \
	SparseHelper.h \
	SgtCore.h \
	SinglePhaseTransformer.h SinglePhaseTransformer.cc \
	SinglePhaseTransformerParserPlugin.h SinglePhaseTransformerParserPlugin.cc \
	Spline.h \
	Stopwatch.h Stopwatch.cc \
	Transformer.h Transformer.cc \
	UndergroundLine.h UndergroundLine.cc \
	UndergroundLineParserPlugin.h UndergroundLineParserPlugin.cc \
	Units.h Units.cc \
	VvTransformer.h VvTransformer.cc \
	VvTransformerParserPlugin.h VvTransformerParserPlugin.cc \
	YamlSupport.h YamlSupport.cc \
	YyTransformer.h YyTransformer.cc \
	YyTransformerParserPlugin.h YyTransformerParserPlugin.cc \
	Zip.h Zip.cc \
	ZipParserPlugin.h ZipParserPlugin.cc

libSgtCore_la_LIBADD = \
	-larmadillo \
	$(BOOST_DATE_TIME_LIB) \
	-lyaml-cpp \
	$(L_KLU_SOLVER) \
	$(L_POWER_TOOLS)

noinst_PROGRAMS = helloworld

helloworld_SOURCES = helloworld.cc
helloworld_LDADD = libSgtCore.la
