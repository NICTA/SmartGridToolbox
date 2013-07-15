AUTOMAKE_OPTIONS = foreign
AM_CPPFLAGS = -I../third_party -I../src

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc
tests_LDADD = ../src/libsmartgridtoolbox.a \
				  ../third_party/yaml-cpp-0.5.0/build/libyaml-cpp.a \
				  ../third_party/SuiteSparse/KLU/Lib/libklu.a \
				  ../third_party/SuiteSparse/AMD/Lib/libamd.a \
				  ../third_party/SuiteSparse/BTF/Lib/libbtf.a \
				  ../third_party/SuiteSparse/COLAMD/Lib/libcolamd.a \
				  -lboost_date_time-mt \
				  -lblas
