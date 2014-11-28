AUTOMAKE_OPTIONS = foreign
TESTS = tests
check_PROGRAMS = tests
tests_SOURCES = tests.cc
tests_LDADD = \
	../SgtCore/libSgtCore.a \
	../SgtSim/libSgtSim.a \
	-larmadillo \
	-lboost_date_time \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf
TESTS_ENVIRONMENT=BOOST_TEST_REPORT_LEVEL=detailed BOOST_TEST_LOG_LEVEL=all
