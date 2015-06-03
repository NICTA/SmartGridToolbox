AUTOMAKE_OPTIONS = foreign

AM_LDFLAGS = -static

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc

tests_LDADD = ../SgtSim/libSgtSim.la ../SgtCore/libSgtCore.la

TESTS_ENVIRONMENT=BOOST_TEST_REPORT_LEVEL=detailed BOOST_TEST_LOG_LEVEL=all
