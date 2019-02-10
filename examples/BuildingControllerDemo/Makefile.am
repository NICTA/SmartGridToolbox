AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = building_controller_demo

building_controller_demo_SOURCES = Building.h Building.cc \
						BuildingController.h BuildingController.cc \
						building_controller_demo.cc

building_controller_demo_LDADD = -lSgtCore -lSgtSim -larmadillo -lboost_date_time -lyaml-cpp -lklu -lamd -lcolamd -lbtf -lgurobi70 -lgurobi_c++
