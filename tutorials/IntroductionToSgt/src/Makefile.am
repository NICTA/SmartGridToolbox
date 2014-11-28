AUTOMAKE_OPTIONS = foreign

LDADD = \
	-lSmartGridToolbox \
	-lboost_date_time \
	-lyaml-cpp \
	-lklu \
	-lamd \
	-lcolamd \
	-lbtf

bin_PROGRAMS = tickers test_overhead_line

tickers_SOURCES = tickers.cc

test_overhead_line_SOURCES = test_overhead_line.cc
