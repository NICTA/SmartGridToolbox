AUTOMAKE_OPTIONS = foreign

libincludedir=$(includedir)/SgtSim

lib_LTLIBRARIES = libSgtSim.la

include_HEADERS = SgtSim.h

libinclude_HEADERS = \
	Battery.h \
	DcPowerSource.h \
	Heartbeat.h \
	Inverter.h \
	SimComponent.h \
	SimNetwork.h \
	SimNetworkComponent.h \
	SimpleBuilding.h \
	SimParser.h \
	Simulation.h \
	SolarPv.h \
	Sun.h \
	TimeSeries.h \
	TimeSeriesZip.h \
	Weather.h

libSgtSim_la_SOURCES = \
	Battery.h Battery.cc \
	BatteryParserPlugin.h BatteryParserPlugin.cc \
	DcPowerSource.h \
	GenericDcPowerSourceParserPlugin.h GenericDcPowerSourceParserPlugin.cc \
	Heartbeat.h \
	HeartbeatParserPlugin.h HeartbeatParserPlugin.cc \
	Inverter.h Inverter.cc \
	InverterParserPlugin.h InverterParserPlugin.cc \
	RegisterParserPlugins.cc \
	SgtSim.h \
	SimBranchParserPlugin.h SimBranchParserPlugin.cc \
	SimBusParserPlugin.h SimBusParserPlugin.cc \
	SimGenParserPlugin.h SimGenParserPlugin.cc \
	SimGlobalParserPlugin.h SimGlobalParserPlugin.cc \
	SimMatpowerParserPlugin.h SimMatpowerParserPlugin.cc \
	SimNetwork.h SimNetwork.cc \
	SimNetworkComponent.h \
	SimNetworkParserPlugin.h SimNetworkParserPlugin.cc \
	SimParser.h SimParser.cc \
	SimpleBuilding.h SimpleBuilding.cc \
	SimpleBuildingParserPlugin.h SimpleBuildingParserPlugin.cc \
	SimSinglePhaseTransformerParserPlugin.h SimSinglePhaseTransformerParserPlugin.cc \
	Simulation.h Simulation.cc \
	SimZipParserPlugin.h SimZipParserPlugin.cc \
	SolarPv.h SolarPv.cc \
	SolarPvParserPlugin.h SolarPvParserPlugin.cc \
	Sun.h Sun.cc \
	TimeSeries.h \
	TimeSeriesParserPlugin.h TimeSeriesParserPlugin.cc \
	TimeSeriesZip.h \
	TimeSeriesZipParserPlugin.h TimeSeriesZipParserPlugin.cc \
	WeakOrder.h WeakOrder.cc \
	Weather.h Weather.cc \
	WeatherParserPlugin.h WeatherParserPlugin.cc

#libSgtSim_la_LIBADD = ../SgtCore/libSgtCore.la
