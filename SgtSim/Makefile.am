AUTOMAKE_OPTIONS = foreign

libincludedir=$(includedir)/SgtCore

lib_LIBRARIES = libSgtSim.a

include_HEADERS = SgtSim.h

libinclude_HEADERS = \
	Battery.h \
	DcPowerSource.h \
	Inverter.h \
	RegularUpdateComponent.h \
	SimComponent.h \
	SimNetwork.h \
	SimNetworkComponent.h \
	SimpleBuilding.h \
	Simulation.h \
	SimParser.h \
	SolarPv.h \
	Spline.h \
	Sun.h \
	TimeSeries.h \
	Weather.h

libSgtSim_a_SOURCES = \
	Battery.h \
	Battery.cc \
	BatteryParserPlugin.h \
	BatteryParserPlugin.cc \
	DcPowerSource.h \
	GenericDcPowerSourceParserPlugin.h \
	GenericDcPowerSourceParserPlugin.cc \
	Inverter.h \
	Inverter.cc \
	InverterParserPlugin.h \
	InverterParserPlugin.cc \
	SgtSim.h \
	SimOverheadLineParserPlugin.h \
	SimOverheadLineParserPlugin.cc \
	RegisterParserPlugins.cc \
	SimBusParserPlugin.h \
	SimBusParserPlugin.cc \
	SimGenericBranchParserPlugin.h \
	SimGenericBranchParserPlugin.cc \
	SimGenericGenParserPlugin.h \
	SimGenericGenParserPlugin.cc \
	SimGenericZipParserPlugin.h \
	SimGenericZipParserPlugin.cc \
	SimComponent.h \
	SimComponent.cc \
	SimDgyTransformerParserPlugin.h \
	SimDgyTransformerParserPlugin.cc \
	SimNetwork.h \
	SimNetwork.cc \
	SimNetworkComponent.h \
	SimNetworkParserPlugin.h \
	SimNetworkParserPlugin.cc \
	SimParser.h \
	SimParser.cc \
	SimpleBuilding.h \
	SimpleBuilding.cc \
	SimpleBuildingParserPlugin.h \
	SimpleBuildingParserPlugin.cc \
	SimGlobalParserPlugin.h \
	SimGlobalParserPlugin.cc \
	SimSinglePhaseTransformerParserPlugin.h \
	SimSinglePhaseTransformerParserPlugin.cc \
	Simulation.h \
	Simulation.cc \
	SimYyTransformerParserPlugin.h \
	SimYyTransformerParserPlugin.cc \
	SolarPv.h \
	SolarPv.cc \
	SolarPvParserPlugin.h \
	SolarPvParserPlugin.cc \
	Sun.h \
	Sun.cc \
	WeakOrder.h \
	WeakOrder.cc \
	Weather.h \
	Weather.cc \
	WeatherParserPlugin.h \
	WeatherParserPlugin.cc

EXTRA_DIST = mainpage.dox
