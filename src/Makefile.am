AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party

lib_LIBRARIES = libsmartgridtoolbox.a
libsmartgridtoolbox_a_SOURCES = \
	BusData.cc Component.cc Meter.cc Model.cc Output.cc Parser.cc Random.cc \
	SimpleBattery.cc SimpleBuilding.cc Simulation.cc solver_nr.cc \
	TestComponent.cc WeakOrder.cc

bin_PROGRAMS =

TESTS = tests
check_PROGRAMS = tests
tests_LDADD = libsmartgridtoolbox.a \
	../third_party/yaml-cpp-0.5.0/build/libyaml-cpp.a \
	../third_party/SuperLU_MT_2.1/libsuperlu_mt_PTHREAD.a
	/usr/local/lib/libboost_date_time-mt.a
tests_SOURCES = tests.cc
