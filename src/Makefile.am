AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a
libsmartgridtoolbox_a_SOURCES = \
	BalancedPowerFlowNR.cc BusData.cc Component.cc Meter.cc Model.cc Output.cc \
	Parser.cc Random.cc SimpleBattery.cc SimpleBuilding.cc Simulation.cc \
	SLUSolver.cc solver_nr.cc TestComponent.cc WeakOrder.cc
bin_PROGRAMS = helloworld
helloworld_SOURCES = helloworld.cc SLUSolver.cc 
helloworld_LDADD = ../third_party/SuperLU_MT_2.1/lib/libsuperlu_mt_PTHREAD.a \
						 -lblas
