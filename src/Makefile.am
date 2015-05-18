AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../SgtCore -I../SgtSim

bin_PROGRAMS = run_config solve_network # network_to_dot

run_config_SOURCES = run_config.cc

# Using LDADD for -l linker flags in this way avoids a potential
# problem with link order.
run_config_LDADD = \
	../SgtSim/libSgtSim.la \
	../SgtCore/libSgtCore.la

solve_network_SOURCES = solve_network.cc

# Using LDADD for -l linker flags in this way avoids a potential
# problem with link order.
solve_network_LDADD = \
	../SgtCore/libSgtCore.la
