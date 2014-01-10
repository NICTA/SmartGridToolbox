AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libSmartGridToolbox.a

pkginclude_HEADERS = Branch.h \
						   Bus.h \
							Common.h \
							Component.h \
							DcPowerSourceBase.h \
							Event.h \
							InverterBase.h \
							Load.h \
							Model.h \
							Network.h \
							Parser.h \
							PowerFlow.h \
							Random.h \
							RegularUpdateComponent.h \
							SimpleBattery.h \
							SimpleBuilding.h \
							SimpleDcPowerSource.h \
							SimpleInverter.h \
							Simulation.h \
							SolarPv.h \
							SparseSolver.h \
							Spline.h \
							Stopwatch.h \
							Sun.h \
							TimeSeries.h \
							Units.h \
							Weather.h \
							ZipToGround.h \
							ZipToGroundBase.h

libSmartGridToolbox_a_SOURCES = Bus.cc \
										  BusParser.cc \
										  Branch.cc \
										  BranchParser.cc \
										  CdfParser.cc \
										  Common.cc \
										  Component.cc \
										  Event.cc \
										  InverterBase.cc \
										  MatpowerParser.cc \
										  Model.cc \
										  Network.cc \
										  NetworkParser.cc \
										  Parser.cc \
										  PowerFlow.cc \
										  PowerFlowNr.cc \
										  Random.cc \
										  RegisterParserPlugins.cc \
										  SimpleBattery.cc \
										  SimpleBatteryParser.cc \
										  SimpleBuilding.cc \
										  SimpleBuildingParser.cc \
										  SimpleDcPowerSourceParser.cc \
										  SimpleInverter.cc \
										  SimpleInverterParser.cc \
										  Simulation.cc \
										  SolarPv.cc \
										  SolarPvParser.cc \
										  SparseSolver.cc \
										  Sun.cc \
										  WeakOrder.cc \
										  Weather.cc \
										  WeatherParser.cc \
										  ZipToGround.cc \
										  ZipToGroundParser.cc

bin_PROGRAMS = run_config helloworld

run_config_SOURCES = run_config.cc
run_config_LDADD = libSmartGridToolbox.a
run_config_LDFLAGS = -lboost_date_time-mt \
							-lblas \
							-lyaml-cpp \
							-lklu \
							-lamd \
							-lcolamd \
							-lbtf

helloworld_SOURCES = helloworld.cc
