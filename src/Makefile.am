AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libsmartgridtoolbox.a
libsmartgridtoolbox_a_SOURCES = Component.cc Meter.cc Model.cc Output.cc \
										  Parser.cc Random.cc SimpleBattery.cc \
										  SimpleBuilding.cc Simulation.cc WeakOrder.cc

bin_PROGRAMS = hello
hello_SOURCES = hello.cc
hello_LDADD = libsmartgridtoolbox.a \
				  ../third_party/yaml-cpp-0.5.0/build/libyaml-cpp.a \
				  /usr/local/lib/libboost_date_time-mt.a

TESTS = tests
check_PROGRAMS = tests
tests_LDADD = libsmartgridtoolbox.a
tests_SOURCES = tests.cc
