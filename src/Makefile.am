AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a
libsmartgridtoolbox_a_SOURCES = PowerFlow1PNR.cc \
										  Bus1P.cc \
										  Branch1P.cc \
										  Common.cc \
										  Component.cc \
										  Meter.cc \
										  Model.cc \
										  Network1P.cc \
										  Output.cc \
										  Parser.cc \
										  Random.cc \
										  SimpleBattery.cc \
										  SimpleBuilding.cc \
										  Simulation.cc \
										  SparseSolver.cc \
										  TestComponent.cc \
										  WeakOrder.cc
bin_PROGRAMS = helloworld klu_simple
helloworld_SOURCES = helloworld.cc
klu_simple_SOURCES = klu_simple.c
helloworld_LDADD = libsmartgridtoolbox.a \
						 ../third_party/SuiteSparse/KLU/Lib/libklu.a \
						 ../third_party/SuiteSparse/AMD/Lib/libamd.a \
						 ../third_party/SuiteSparse/BTF/Lib/libbtf.a \
						 ../third_party/SuiteSparse/COLAMD/Lib/libcolamd.a \
						 -lblas
klu_simple_LDADD = \
						 ../third_party/SuiteSparse/KLU/Lib/libklu.a \
						 ../third_party/SuiteSparse/AMD/Lib/libamd.a \
						 ../third_party/SuiteSparse/BTF/Lib/libbtf.a \
						 ../third_party/SuiteSparse/COLAMD/Lib/libcolamd.a \
						 -lblas
