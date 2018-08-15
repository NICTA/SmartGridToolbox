AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS += -I../SgtSim
AM_LDFLAGS = -static

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc

tests_LDADD = ../SgtSim/libSgtSim.la ../SgtCore/libSgtCore.la -lpthread

TESTS_ENVIRONMENT=BOOST_TEST_REPORT_LEVEL=detailed BOOST_TEST_LOG_LEVEL=all

bin_PROGRAMS = debug_matpower

debug_matpower_SOURCES = debug_matpower.cc

debug_matpower_LDADD = ../SgtCore/libSgtCore.la
