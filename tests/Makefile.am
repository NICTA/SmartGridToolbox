AUTOMAKE_OPTIONS = foreign
AM_CPPFLAGS = -I../third_party -I../src

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc
tests_LDADD = \
	../src/libsmartgridtoolbox.a \
	../third_party/yaml-cpp-0.5.0/build/libyaml-cpp.a \
	../third_party/SuperLU_MT_2.1/lib/libsuperlu_mt_PTHREAD.a \
	/usr/local/lib/libboost_date_time-mt.a
