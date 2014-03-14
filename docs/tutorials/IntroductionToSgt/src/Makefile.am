AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = helloworld

# Using LDADD for -l linker flags in this way avoids a potential problem with link order.
helloworld_LDADD = ../Template/libTemplate.a

helloworld_SOURCES = helloworld.cc
