AUTOMAKE_OPTIONS = foreign

libincludedir = $(includedir)/SgtSim

lib_LTLIBRARIES = libSgtSim.la

include_HEADERS = SgtSim.h

libinclude_HEADERS = \
	Battery.h \
	DcPowerSource.h \
	Heartbeat.h \
	Inverter.h \
	RealTimeClock.h \
	SimComponent.h \
	SimNetwork.h \
	SimpleBuilding.h \
	SimParser.h \
	Simulation.h \
	SolarGeom.h \
	SolarPv.h \
	Sun.h \
	TapChanger.h \
	TimeSeries.h \
	TimeSeriesDcPowerSource.h \
	TimeSeriesZip.h \
	Weather.h

libSgtSim_la_SOURCES = \
	Battery.h Battery.cc \
	BatteryParserPlugin.h BatteryParserPlugin.cc \
	DcPowerSource.h DcPowerSource.cc \
	GenericDcPowerSourceParserPlugin.h GenericDcPowerSourceParserPlugin.cc \
	Heartbeat.h \
	HeartbeatParserPlugin.h HeartbeatParserPlugin.cc \
	Inverter.h Inverter.cc \
	InverterParserPlugin.h InverterParserPlugin.cc \
	RealTimeClock.h RealTimeClock.cc \
	RegisterParserPlugins.cc \
	SgtSim.h \
	SimComponent.h SimComponent.cc \
	SimGlobalParserPlugin.h SimGlobalParserPlugin.cc \
	SimMatpowerParserPlugin.h SimMatpowerParserPlugin.cc \
	SimNetwork.h SimNetwork.cc \
	SimNetworkParserPlugin.h SimNetworkParserPlugin.cc \
	SimParser.h SimParser.cc \
	SimpleBuilding.h SimpleBuilding.cc \
	SimpleBuildingParserPlugin.h SimpleBuildingParserPlugin.cc \
	Simulation.h Simulation.cc \
	SolarGeom.h SolarGeom.cc \
	SolarPv.h SolarPv.cc \
	SolarPvParserPlugin.h SolarPvParserPlugin.cc \
	Sun.h Sun.cc \
	TapChanger.h TapChanger.cc \
	TapChangerParserPlugin.h TapChangerParserPlugin.cc \
	TimeSeries.h \
	TimeSeriesDcPowerSource.h \
	TimeSeriesDcPowerSourceParserPlugin.h TimeSeriesDcPowerSourceParserPlugin.cc \
	TimeSeriesParserPlugin.h TimeSeriesParserPlugin.cc \
	TimeSeriesZip.h TimeSeriesZip.cc \
	TimeSeriesZipParserPlugin.h TimeSeriesZipParserPlugin.cc \
	WeakOrder.h WeakOrder.cc \
	Weather.h Weather.cc \
	WeatherParserPlugin.h WeatherParserPlugin.cc
