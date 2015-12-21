AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS =

lib_LIBRARIES = 

bin_PROGRAMS = pvdemo

pvdemo_SOURCES = PvInverter.h PvInverter.cc PvDemoSolver.h PvDemoSolver.cc pvdemo.cc
pvdemo_LDADD = -lSgtCore -lSgtSim -larmadillo -lyaml-cpp -lPowerTools++ -lipopt -lcoinhsl -lboost_date_time -lklu -lamd -lbtf -lcolamd -lgurobi65 -lgurobi_c++
