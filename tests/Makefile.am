AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I../src -I$(pkgincludedir)
AM_LDFLAGS = -L$(libdir)

TESTS = tests

check_PROGRAMS = tests

tests_SOURCES = tests.cc
tests_LDFLAGS = $(AM_LDFLAGS) \
					 -lsmartgridtoolbox \
					 -lboost_date_time-mt \
					 -lblas \
					 -lyaml-cpp \
					 -lklu \
					 -lamd \
					 -lcolamd \
					 -lbtf
