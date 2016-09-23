AUTOMAKE_OPTIONS = foreign

libincludedir = $(includedir)/SgtSim

lib_LTLIBRARIES = libSgtSim.la

include_HEADERS = SgtSim.h

libinclude_HEADERS = \
	Battery.h \
	DcPowerSource.h \
	Heartbeat.h \
	Inverter.h \
	SimBranch.h \
	SimBus.h \
	SimComponent.h \
	SimGen.h \
	SimNetwork.h \
	SimpleBuilding.h \
	SimParser.h \
	SimZip.h \
	Simulation.h \
	SolarGeom.h \
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
	SimBus.h SimBus.cc \
	SimBusParserPlugin.h SimBusParserPlugin.cc \
	SimBranch.h SimBranch.cc \
	SimBranchParserPlugin.h SimBranchParserPlugin.cc \
	SimComponent.h SimComponent.cc \
	SimGen.h SimGen.cc \
	SimGenParserPlugin.h SimGenParserPlugin.cc \
	SimGlobalParserPlugin.h SimGlobalParserPlugin.cc \
	SimMatpowerParserPlugin.h SimMatpowerParserPlugin.cc \
	SimNetwork.h SimNetwork.cc \
	SimNetworkParserPlugin.h SimNetworkParserPlugin.cc \
	SimParser.h SimParser.cc \
	SimpleBuilding.h SimpleBuilding.cc \
	SimpleBuildingParserPlugin.h SimpleBuildingParserPlugin.cc \
	Simulation.h Simulation.cc \
	SimZip.h SimZip.cc \
	SimZipParserPlugin.h SimZipParserPlugin.cc \
	SolarGeom.h SolarGeom.cc \
	SolarPv.h SolarPv.cc \
	SolarPvParserPlugin.h SolarPvParserPlugin.cc \
	Sun.h Sun.cc \
	TimeSeries.h \
	TimeSeriesParserPlugin.h TimeSeriesParserPlugin.cc \
	TimeSeriesZip.h TimeSeriesZip.cc \
	TimeSeriesZipParserPlugin.h TimeSeriesZipParserPlugin.cc \
	WeakOrder.h WeakOrder.cc \
	Weather.h Weather.cc \
	WeatherParserPlugin.h WeatherParserPlugin.cc
