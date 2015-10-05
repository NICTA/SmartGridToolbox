AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS =

lib_LIBRARIES = 

bin_PROGRAMS = building_controller_demo

building_controller_demo_SOURCES = Building.h Building.cc \
						BuildingController.h BuildingController.cc \
						building_controller_demo.cc

building_controller_demo_LDADD = -lSgtCore \
					  -lSgtSim \
					  -larmadillo \
					  -lyaml-cpp \
					  -lPowerTools++ \
					  -lipopt \
					  -lcoinhsl \
					  -lboost_date_time \
					  -lklu \
					  -lamd \
					  -lbtf \
					  -lcolamd \
					  -lgurobi60
