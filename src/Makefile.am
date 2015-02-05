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

AM_CPPFLAGS = -I../SgtCore -I../SgtSim

bin_PROGRAMS = run_config solve_network # network_to_dot

run_config_SOURCES = run_config.cc

run_config_LDFLAGS = \
	$(FRAMEWORK_FOUNDATION) \
	$(FRAMEWORK_POWER_TOOLS)

# Using LDADD for -l linker flags in this way avoids a potential
# problem with link order.
run_config_LDADD = \
	../SgtSim/libSgtSim.a \
	../SgtCore/libSgtCore.a \
	-lboost_date_time \
	-larmadillo \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf \
	$(L_IPOPT) \
	$(L_COIN_HSL)

solve_network_SOURCES = solve_network.cc

solve_network_LDFLAGS = \
	$(FRAMEWORK_FOUNDATION) \
	$(FRAMEWORK_POWER_TOOLS)

# Using LDADD for -l linker flags in this way avoids a potential
# problem with link order.
solve_network_LDADD = \
	../SgtCore/libSgtCore.a \
	-lboost_date_time \
	-larmadillo \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf \
	$(L_IPOPT) \
	$(L_COIN_HSL)

# network_to_dot_SOURCES = network_to_dot.cc
# 
# network_to_dot_LDADD = \
#	../SgtSim/libSgtSim.a \
#	../SgtCore/libSgtCore.a \
#	-lboost_date_time \
#	-lyaml-cpp \
#	-lklu \
#	-lamd \
#	-lcolamd \
#	-lbtf
