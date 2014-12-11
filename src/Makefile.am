AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../SgtCore -I../SgtSim

bin_PROGRAMS = run_config solve_network to_json # network_to_dot

run_config_SOURCES = run_config.cc

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
	-lbtf

solve_network_SOURCES = solve_network.cc

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
	-lbtf

to_json_SOURCES = NetworkGraph.h NetworkGraph.cc to_json.cc

# Using LDADD for -l linker flags in this way avoids a potential
# problem with link order.
to_json_LDADD = \
	../SgtSim/libSgtSim.a \
	../SgtCore/libSgtCore.a \
	-lboost_date_time \
	-larmadillo \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf \
	-logdf

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
