AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party -I../src

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc
tests_LDADD = ../src/libsmartgridtoolbox.a \
				  ../third_party/SuiteSparse/KLU/Lib/libklu.a \
				  ../third_party/SuiteSparse/AMD/Lib/libamd.a \
				  ../third_party/SuiteSparse/COLAMD/Lib/libcolamd.a \
				  ../third_party/SuiteSparse/BTF/Lib/libbtf.a
tests_LDFLAGS = -lboost_date_time-mt \
					 -lblas \
					 -lyaml-cpp
