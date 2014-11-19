AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = run_config solve_network network_to_dot

run_config_SOURCES = run_config.cc

# Using LDADD for -l linker flags in this way avoids a potential
# problem with link order.
run_config_LDADD = \
	../SgtCore/libSgtCore.a \
	../SgtSim/libSgtSim.a \
	-lboost_date_time \
	-lblas \
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
	-lblas \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf

# network_to_dot_SOURCES = network_to_dot.cc
# 
# network_to_dot_LDADD = \
#	../SgtCore/libSgtCore.a \
#	../SgtSim/libSgtSim.a \
#	-lboost_date_time \
#	-lblas \
#	-lyaml-cpp \
#	-lklu \
#	-lamd \
#	-lcolamd \
#	-lbtf
