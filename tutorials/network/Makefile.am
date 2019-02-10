AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = network network_yaml

network_SOURCES = network.cc
network_LDADD = -lSgtCore -larmadillo -lboost_date_time -lyaml-cpp -lklu -lamd -lcolamd -lbtf

network_yaml_SOURCES = network_yaml.cc
network_yaml_LDADD = -lSgtCore -larmadillo -lboost_date_time -lyaml-cpp -lklu -lamd -lcolamd -lbtf
