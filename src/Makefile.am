AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a

libsmartgridtoolbox_a_SOURCES = PowerFlow1PNR.cc \
										  Bus1P.cc \
										  Branch1P.cc \
										  Common.cc \
										  Component.cc \
										  Event.cc \
										  Meter.cc \
										  Model.cc \
										  Network1P.cc \
										  Parser.cc \
										  Random.cc \
										  RegisterComponentParsers.cc \
										  SimpleBattery.cc \
										  SimpleBuilding.cc \
										  Simulation.cc \
										  SparseSolver.cc \
										  TestComponent.cc \
										  WeakOrder.cc \
										  ZipToGround1P.cc

bin_PROGRAMS = run_config

run_config_SOURCES = run_config.cc
run_config_LDADD = libsmartgridtoolbox.a \
						 -L ../third_party/SuiteSparse/KLU \
						 -L ../third_party/SuiteSparse/AMD \
						 -L ../third_party/SuiteSparse/BTF \
						 -L ../third_party/SuiteSparse/COLAMD \
						 -lyaml-cpp \
						 -lklu \
						 -lamd \
						 -lcolamd \
						 -lbtf \
						 -lboost_date_time-mt \
						 -lblas
