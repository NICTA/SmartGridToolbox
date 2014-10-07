AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = helloworld run_config network_to_dot

helloworld_SOURCES = helloworld.cc

run_config_SOURCES = run_config.cc

# Using LDADD for -l linker flags in this way avoids a potential
# problem with link order.
run_config_LDADD = \
	../SgtCore/libSgtCore.a \
	../SgtSim/libSgtSim.a \
	-lboost_date_time-mt \
	-lblas \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf

network_to_dot_SOURCES = network_to_dot.cc

network_to_dot_LDADD = \
	../SgtCore/libSgtCore.a \
	../SgtSim/libSgtSim.a \
	-lboost_date_time-mt \
	-lblas \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf
