AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libsmartgridtoolbox.a
libsmartgridtoolbox_a_SOURCES = Component.cc Meter.cc Model.cc Output.cc \
										  Parser.cc Random.cc SimpleBattery.cc \
										  SimpleBuilding.cc Simulation.cc \
										  TestComponent.cc WeakOrder.cc

bin_PROGRAMS =

TESTS = tests
check_PROGRAMS = tests
tests_LDADD = libsmartgridtoolbox.a \
				  ../third_party/yaml-cpp-0.5.0/build/libyaml-cpp.a \
				  /usr/local/lib/libboost_date_time-mt.a
tests_SOURCES = tests.cc
