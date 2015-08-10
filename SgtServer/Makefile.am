AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS += -I../SgtSim -I../SgtCore

libincludedir = $(includedir)/SgtServer

#lib_LTLIBRARIES = libSgtServer.la

include_HEADERS = SgtServer.h

libinclude_HEADERS = \
	client_http.hpp \
	client_https.hpp \
	server_http.hpp \
	server_https.hpp

#libSgtServer_la_SOURCES = \
#	client_http.hpp \
#	client_https.hpp \
#	server_http.hpp \
#	server_https.hpp \
#	SgtServer.h

bin_PROGRAMS = test_sgt_server

test_sgt_server_SOURCES = test_sgt_server.cc

test_sgt_server_LDADD = ../SgtSim/libSgtSim.la ../SgtCore/libSgtCore.la -lboost_system-mt -lboost_thread-mt -lboost_coroutine-mt -lboost_context-mt -lboost_filesystem-mt
