AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a

libsmartgridtoolbox_a_SOURCES = Bus.cc \
										  Bus1P.cc \
										  Branch.cc \
										  Branch1P.cc \
										  Common.cc \
										  Component.cc \
										  Event.cc \
										  Meter.cc \
										  Model.cc \
										  Network.cc \
										  Network1P.cc \
										  Parser.cc \
										  PowerFlow.cc \
										  PowerFlowNR.cc \
										  PowerFlow1PNR.cc \
										  Random.cc \
										  RegisterComponentParsers.cc \
										  SimpleBattery.cc \
										  SimpleBuilding.cc \
										  Simulation.cc \
										  SparseSolver.cc \
										  TestComponent.cc \
										  WeakOrder.cc \
										  ZipToGround.cc \
										  ZipToGround1P.cc

bin_PROGRAMS = run_config

run_config_SOURCES = run_config.cc
run_config_LDADD = libsmartgridtoolbox.a \
						 ../third_party/SuiteSparse/KLU/Lib/libklu.a \
						 ../third_party/SuiteSparse/AMD/Lib/libamd.a \
						 ../third_party/SuiteSparse/COLAMD/Lib/libcolamd.a \
						 ../third_party/SuiteSparse/BTF/Lib/libbtf.a \
						 ../third_party/yaml-cpp-0.5.0/build/libyaml-cpp.a
run_config_LDFLAGS = -lboost_date_time-mt \
							-lblas
