AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = run_config helloworld

run_config_SOURCES = run_config.cc
# Using LDADD for -l linker flags in this way avoids a potential
# problem with link order.
run_config_LDADD = \
	../SmartGridToolbox/libSmartGridToolbox.a \
	-lboost_date_time-mt \
	-lblas \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf

helloworld_SOURCES = helloworld.cc
