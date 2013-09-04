AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a

libsmartgridtoolbox_a_SOURCES = Bus.cc \
										  Branch.cc \
										  Common.cc \
										  Component.cc \
										  Event.cc \
										  InverterBase.cc \
										  Model.cc \
										  Network.cc \
										  Parser.cc \
										  PowerFlow.cc \
										  PowerFlowNR.cc \
										  Random.cc \
										  RegisterComponentParsers.cc \
										  SimpleBattery.cc \
										  SimpleBuilding.cc \
										  SimpleDCPowerSource.cc \
										  SimpleInverter.cc \
										  Simulation.cc \
										  SolarPV.cc \
										  SparseSolver.cc \
										  Sun.cc \
										  TestComponent.cc \
										  WeakOrder.cc \
										  Weather.cc \
										  ZipToGround.cc

bin_PROGRAMS = run_config helloworld

run_config_SOURCES = run_config.cc
run_config_LDADD = libsmartgridtoolbox.a \
						 ../third_party/SuiteSparse/KLU/Lib/libklu.a \
						 ../third_party/SuiteSparse/AMD/Lib/libamd.a \
						 ../third_party/SuiteSparse/COLAMD/Lib/libcolamd.a \
						 ../third_party/SuiteSparse/BTF/Lib/libbtf.a \
						 ../third_party/yaml-cpp-0.5.0/build/libyaml-cpp.a
run_config_LDFLAGS = -lboost_date_time-mt \
							-lblas

helloworld_SOURCES = helloworld.cc \
							Common.cc
helloworld_LDFLAGS = -lboost_date_time-mt \
							-lblas
