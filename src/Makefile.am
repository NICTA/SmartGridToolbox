AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a
libsmartgridtoolbox_a_SOURCES = \
	BusData.cc Component.cc Meter.cc Model.cc Output.cc Parser.cc Random.cc \
	SimpleBattery.cc SimpleBuilding.cc Simulation.cc solver_nr.cc \
	TestComponent.cc WeakOrder.cc
bin_PROGRAMS = helloworld
helloworld_SOURCES = helloworld.cc
