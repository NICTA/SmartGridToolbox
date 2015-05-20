AUTOMAKE_OPTIONS = foreign

AM_LDFLAGS = -all-static

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc

tests_LDADD = ../SgtCore/libSgtCore.la ../SgtSim/libSgtSim.la

TESTS_ENVIRONMENT=BOOST_TEST_REPORT_LEVEL=detailed BOOST_TEST_LOG_LEVEL=all
