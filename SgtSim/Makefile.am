AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libSgtSim.a

include_HEADERS = LibSgtSim.h

pkginclude_HEADERS = \
	Battery.h \
	DcPowerSourceBase.h \
	GenericDcPowerSource.h \
	Inverter.h \
	InverterBase.h \
	RegularUpdateComponent.h \
	SimComponent.h \
	SimNetwork.h \
	SimNetworkComponent.h \
	SimpleBuilding.h \
	SimSinglePhaseTransformer.h \
	Simulation.h \
	SolarPv.h \
	Spline.h \
	Sun.h \
	TimeSeries.h \
	Weather.h \
	YyTransformer.h

libSgtSim_a_SOURCES = \
	Battery.h \
	Battery.cc \
	Battery.h \
	Battery.cc \
	DcPowerSourceBase.h \
	GenericDcPowerSource.h \
	GenericDcPowerSourceParser.h \
	GenericDcPowerSourceParser.cc \
	Inverter.h \
	Inverter.cc \
	InverterParser.h \
	InverterParser.cc \
	InverterBase.h \
	InverterBase.cc \
	LibSgtSim.h \
	SimOverheadLineParser.h \
	SimOverheadLineParser.cc \
	RegisterParserPlugins.cc \
	SimGenericBranchParser.h \
	SimGenericBranchParser.cc \
	SimBusParser.h \
	SimBusParser.cc \
	SimComponent.h \
	SimComponent.cc \
	SimDgyTransformerParser.h \
	SimDgyTransformerParser.cc \
	SimNetwork.h \
	SimNetwork.cc \
	SimNetworkComponent.h \
	SimNetworkParser.h \
	SimNetworkParser.cc \
	SimZipParser.h \
	SimZipParser.cc \
	SimpleBuilding.h \
	SimpleBuilding.cc \
	SimpleBuildingParser.h \
	SimpleBuildingParser.cc \
	SimSinglePhaseTransformer.h \
	SimSinglePhaseTransformer.cc \
	SimSinglePhaseTransformerParser.h \
	SimSinglePhaseTransformerParser.cc \
	Simulation.h \
	Simulation.cc \
	SolarPv.h \
	SolarPv.cc \
	SolarPvParser.h \
	SolarPvParser.cc \
	Sun.h \
	Sun.cc \
	WeakOrder.h \
	WeakOrder.cc \
	Weather.h \
	Weather.cc \
	WeatherParser.h \
	WeatherParser.cc \
	YyTransformer.h \
	YyTransformer.cc \
	YyTransformerParser.h \
	YyTransformerParser.cc

EXTRA_DIST = mainpage.dox
