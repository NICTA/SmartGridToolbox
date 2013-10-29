AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a

pkginclude_HEADERS = Branch.h \
						   Bus.h \
							Common.h \
							Component.h \
							DCPowerSourceBase.h \
							Event.h \
							InverterBase.h \
							Load.h \
							Model.h \
							Network.h \
							Parser.h \
							PowerFlow.h \
							Property.h \
							Random.h \
							RegularUpdateComponent.h \
							SimpleBattery.h \
							SimpleBuilding.h \
							SimpleDCPowerSource.h \
							SimpleInverter.h \
							Simulation.h \
							SolarPV.h \
							SparseSolver.h \
							Spline.h \
							Stopwatch.h \
							Sun.h \
							TimeSeries.h \
							Units.h \
							Weather.h \
							ZipToGround.h \
							ZipToGroundBase.h

libsmartgridtoolbox_a_SOURCES = Bus.cc \
										  Branch.cc \
										  CDFParser.cc \
										  Common.cc \
										  Component.cc \
										  Event.cc \
										  InverterBase.cc \
										  MatpowerParser.cc \
										  Model.cc \
										  Network.cc \
										  Parser.cc \
										  PowerFlow.cc \
										  PowerFlowNR.cc \
										  Random.cc \
										  RegisterParserPlugins.cc \
										  SimpleBattery.cc \
										  SimpleBuilding.cc \
										  SimpleDCPowerSource.cc \
										  SimpleInverter.cc \
										  Simulation.cc \
										  SolarPV.cc \
										  SparseSolver.cc \
										  Sun.cc \
										  WeakOrder.cc \
										  Weather.cc \
										  ZipToGround.cc

bin_PROGRAMS = run_config helloworld

run_config_SOURCES = run_config.cc
run_config_LDADD = libsmartgridtoolbox.a \
						 ../third_party/SuiteSparse/KLU/Lib/libklu.a \
						 ../third_party/SuiteSparse/AMD/Lib/libamd.a \
						 ../third_party/SuiteSparse/COLAMD/Lib/libcolamd.a \
						 ../third_party/SuiteSparse/BTF/Lib/libbtf.a
run_config_LDFLAGS = -lboost_date_time-mt \
							-lblas \
							-lyaml-cpp

helloworld_SOURCES = helloworld.cc \
							Common.cc
helloworld_LDFLAGS = -lboost_date_time-mt \
							-lblas
