AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS += -I../.. -I/usr/local/opt/openssl/include

AM_LDFLAGS = -L/usr/local/opt/openssl/lib

bin_PROGRAMS = sgt_server

sgt_server_SOURCES = sgt_server.cc

sgt_server_LDADD = ../../SgtSim/libSgtSim.la ../../SgtCore/libSgtCore.la -lcpprest -lcrypto -lssl $(BOOST_CHRONO_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_REGEX_LIB) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) 
