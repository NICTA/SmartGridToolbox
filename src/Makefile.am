AUTOMAKE_OPTIONS = foreign

CFLAGS=
CPPFLAGS=
LDFLAGS=

bin_PROGRAMS = hello
hello_SOURCES = Model.cc Output.cc Parser.cc Random.cc Simulation.cc \
					 WeakOrder.cc hello.cc
hello_LDADD = ../third_party/yaml-cpp-0.5.0/build/libyaml-cpp.a \
				  /usr/local/lib/libboost_date_time-mt.a
TESTS = tests
check_PROGRAMS = tests
tests_SOURCES = WeakOrder.cc tests.cc
