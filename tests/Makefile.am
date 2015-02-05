AUTOMAKE_OPTIONS = foreign

if ENABLE_POWER_TOOLS
FRAMEWORK_FOUNDATION = -framework Foundation 
FRAMEWORK_POWER_TOOLS = -framework PowerTools 
L_IPOPT = -lipopt
L_COIN_HSL = -lcoinhsl
else
FRAMEWORK_FOUNDATION =
FRAMEWORK_POWER_TOOLS =
L_IPOPT =
L_COIN_HSL =
endif

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc

tests_LDFLAGS = \
	$(FRAMEWORK_FOUNDATION) \
	$(FRAMEWORK_POWER_TOOLS)

tests_LDADD = \
	../SgtCore/libSgtCore.a \
	../SgtSim/libSgtSim.a \
	-larmadillo \
	-lboost_date_time \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf \
	$(L_IPOPT) \
	$(L_COIN_HSL)

TESTS_ENVIRONMENT=BOOST_TEST_REPORT_LEVEL=detailed BOOST_TEST_LOG_LEVEL=all
