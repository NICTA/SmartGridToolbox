cmake_minimum_required(VERSION 3.2)

project(PowerTools)

# The version number.
set (PowerTools_VERSION_MAJOR 1)
set (PowerTools_VERSION_MINOR 0)

# Find Gurobi (optional).
option(ENABLE_GUROBI "Link to Gurobi libraries" OFF)

# configure a header file to pass some of the CMake settings
# to the source code.
configure_file (
    "${PROJECT_SOURCE_DIR}/PowerToolsConfig.h.in"
    "${PROJECT_BINARY_DIR}/include/PowerTools++/PowerToolsConfig.h")

# add the binary tree to the search path for include files
# so that we will find PowerToolsConfig.h.
include_directories("${PROJECT_BINARY_DIR}/include")
 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Find Gurobi?
if (ENABLE_GUROBI)
    message(STATUS "Enable GUROBI")
    add_definitions(-DUSE_GUROBI)
    find_package(GUROBI)
    include_directories(${GUROBI_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${GUROBI_LIBRARIES})
endif()

# Find Ipopt.
find_package(IPOPT REQUIRED)
include_directories(${IPOPT_INCLUDE_DIRS}/..)
set(LIBS ${LIBS} ${IPOPT_LIBRARIES})

# Compiler options

# specify the cross compiler
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")
#string (REPLACE "-Wmost" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")

# Build options
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_BUILD_TYPE "Debug")

add_subdirectory(src)

# Install
install(DIRECTORY include/PowerTools++ DESTINATION include)
install(FILES ${PROJECT_BINARY_DIR}/include/PowerTools++/PowerToolsConfig.h DESTINATION include/PowerTools++)
