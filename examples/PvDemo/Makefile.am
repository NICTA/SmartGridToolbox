AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = pvdemo

pvdemo_SOURCES = PvInverter.h PvInverter.cc PvDemoSolver.h PvDemoSolver.cc pvdemo.cc

pvdemo_LDADD = -lSgtCore -lSgtSim -larmadillo -lboost_date_time -lyaml-cpp -lklu -lamd -lcolamd -lbtf -lgurobi70 -lgurobi_c++ -lipopt -lcoinhsl
