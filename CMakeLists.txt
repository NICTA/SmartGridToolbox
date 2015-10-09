cmake_minimum_required(VERSION 3.2)

project(PowerTools)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Find gurobi and ipopt on system
find_package(GUROBI REQUIRED)
find_package(IPOPT REQUIRED)

# Include gurobi and ipopt headers and libraries
include_directories(${GUROBI_INCLUDE_DIRS} ${IPOPT_INCLUDE_DIRS}/..)
set(LIBS ${LIBS} ${GUROBI_LIBRARIES} ${IPOPT_LIBRARIES})

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
