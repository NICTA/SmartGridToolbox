AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS += -I../.. -I/usr/local/opt/openssl/include

AM_LDFLAGS = -L/usr/local/opt/openssl/lib

bin_PROGRAMS = sgt_server

sgt_server_SOURCES = sgt_server.cc

sgt_server_LDADD = -lSgtSim -lSgtCore -lcpprest -lcrypto -lssl -lboost_thread-mt -lboost_system -lboost_filesystem -lboost_chrono
