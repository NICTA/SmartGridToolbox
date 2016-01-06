AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS += -I../../SgtSim -I../../SgtCore -I/usr/local/opt/openssl/include

AM_LDFLAGS = -L/usr/local/opt/openssl/lib

bin_PROGRAMS = test_cpp_rest

test_cpp_rest_SOURCES = test_cpp_rest.cc

#test_cpp_rest_LDADD = ../SgtSim/libSgtSim.la ../SgtCore/libSgtCore.la -lboost_system-mt -lboost_thread-mt -lboost_coroutine-mt -lboost_context-mt -lboost_filesystem-mt -lcpprest -lboost_system -lcrypto -lssl -levent_openssl -lboost_chrono -levent
test_cpp_rest_LDADD = -lSgtSim -lSgtCore -lcpprest -lcrypto -lssl -lboost_thread-mt -lboost_system -lboost_chrono
