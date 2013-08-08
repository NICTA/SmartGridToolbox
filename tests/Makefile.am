AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../third_party -I../src

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc
tests_LDADD = -L ../src \
				  -L ../third_party/yaml-cpp-0.5.0/build \
				  -L ../third_party/SuiteSparse/KLU \
				  -L ../third_party/SuiteSparse/AMD \
				  -L ../third_party/SuiteSparse/BTF \
				  -L ../third_party/SuiteSparse/COLAMD \
				  -lsmartgridtoolbox \
				  -lyaml-cpp \
				  -lklu \
				  -lamd \
				  -lcolamd \
				  -lbtf \
				  -lboost_date_time-mt \
				  -lblas
