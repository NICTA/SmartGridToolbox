AUTOMAKE_OPTIONS = foreign
TESTS = tests
check_PROGRAMS = tests
tests_SOURCES = tests.cc
tests_LDADD = \
	../SmartGridToolbox/libSmartGridToolbox.a \
	-lboost_date_time-mt \
	-lblas \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf
TESTS_ENVIRONMENT=BOOST_TEST_REPORT_LEVEL=detailed BOOST_TEST_LOG_LEVEL=all
