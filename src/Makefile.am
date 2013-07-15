AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a
libsmartgridtoolbox_a_SOURCES = \
	BalancedPowerFlowNR.cc BusData.cc Component.cc Meter.cc Model.cc Output.cc \
	Parser.cc Random.cc SimpleBattery.cc SimpleBuilding.cc Simulation.cc \
	SLUSolver.cc TestComponent.cc WeakOrder.cc
bin_PROGRAMS = helloworld klu_simple
helloworld_SOURCES = SLUSolver.cc helloworld.cc
klu_simple_SOURCES = SLUSolver.cc klu_simple.c
helloworld_LDADD = ../third_party/SuperLU_MT_2.1/lib/libsuperlu_mt_PTHREAD.a \
						 ../third_party/SuiteSparse/KLU/Lib/libklu.a \
						 ../third_party/SuiteSparse/AMD/Lib/libamd.a \
						 ../third_party/SuiteSparse/BTF/Lib/libbtf.a \
						 -lblas
klu_simple_LDADD = ../third_party/SuperLU_MT_2.1/lib/libsuperlu_mt_PTHREAD.a \
						 ../third_party/SuiteSparse/KLU/Lib/libklu.a \
						 ../third_party/SuiteSparse/AMD/Lib/libamd.a \
						 ../third_party/SuiteSparse/BTF/Lib/libbtf.a \
						 -lblas
