AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = powerflow_1p_a
powerflow_1p_a_SOURCES = powerflow_1p_a.cc
powerflow_1p_LDADD = ../libsmartgridtoolbox.a
							../third_party/SuiteSparse/KLU/Lib/libklu.a \
							../third_party/SuiteSparse/AMD/Lib/libamd.a \
							../third_party/SuiteSparse/BTF/Lib/libbtf.a \
							../third_party/SuiteSparse/COLAMD/Lib/libcolamd.a \
							-lblas
