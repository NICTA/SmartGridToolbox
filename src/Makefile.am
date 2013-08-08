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
										  SimpleBattery.cc \
										  SimpleBuilding.cc \
										  Simulation.cc \
										  SparseSolver.cc \
										  TestComponent.cc \
										  WeakOrder.cc

bin_PROGRAMS = helloworld

helloworld_SOURCES = helloworld.cc
helloworld_LDADD = -L . \
						 -L ../third_party/SuiteSparse/KLU \
						 -L ../third_party/SuiteSparse/AMD \
						 -L ../third_party/SuiteSparse/BTF \
						 -L ../third_party/SuiteSparse/COLAMD \
						 -lsmartgridtoolbox \
						 -lklu \
						 -lamd \
						 -lbtf \
						 -lblas
