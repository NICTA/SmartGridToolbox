##set(IPOPT_ROOT_DIR "$ENV{IPOPT_ROOT_DIR}" CACHE PATH "IPOPT root directory.")
#set(IPOPT_ROOT_DIR "/usr/local" CACHE PATH "IPOPT root directory.")
message("Looking for Ipopt in ${IPOPT_ROOT_DIR}")

#string(REGEX MATCH "[0-9]+" IPOPT_VERSION "${IPOPT_ROOT_DIR}")

#message("Ipopt version ${IPOPT_VERSION}")

#string(SUBSTRING ${IPOPT_VERSION} 0 2 IPOPT_VERSION_SHORT)

find_path(IPOPT_INCLUDE_DIR
	NAMES IpNLP.hpp 
	HINTS /usr/local/include/coin
	HINTS ${IPOPT_ROOT_DIR}/include/coin
	HINTS ${IPOPT_ROOT_DIR}/include
)

if(APPLE)
find_library(IPOPT_LIBRARY 
	libipopt.dylib
	HINTS /usr/local/lib
	HINTS ${IPOPT_ROOT_DIR}/lib
)
elseif(UNIX)
find_library(IPOPT_LIBRARY 
	libipopt.so 
	HINTS /usr/local/lib
	HINTS ${IPOPT_ROOT_DIR}/lib
)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IPOPT DEFAULT_MSG IPOPT_LIBRARY IPOPT_INCLUDE_DIR)

if(IPOPT_FOUND)
    set(IPOPT_INCLUDE_DIRS ${IPOPT_INCLUDE_DIR})
    set(IPOPT_LIBRARIES ${IPOPT_LIBRARY})
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(IPOPT_LIBRARIES "${IPOPT_LIBRARIES};m;pthread")
    endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
endif(IPOPT_FOUND)

mark_as_advanced(IPOPT_LIBRARY IPOPT_INCLUDE_DIR)
