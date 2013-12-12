AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(includedir)
AM_LDFLAGS = -L$(libdir)

lib_LIBRARIES = 

bin_PROGRAMS = pvdemo

pvdemo_SOURCES = pvdemo.cc PVDemoController.cc
pvdemo_LDFLAGS = -lSmartGridToolbox -lyaml-cpp -lboost_date_time-mt -lklu -lamd -lbtf -lcolamd -lgurobi55
