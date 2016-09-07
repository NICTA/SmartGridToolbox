include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB HEADERS ${PROJECT_INCLUDE_DIR} *.h)
file(GLOB SOURCES . *.cpp ${PROJECT_INCLUDE_DIR}/PowerTools++/*.h)
if (NOT ENABLE_GUROBI)
  list(REMOVE_ITEM SOURCES ${PROJECT_SOURCE_DIR}/src/GurobiProgram.cpp)
endif()

find_library(BLAS_LIBRARIES NAMES blas coinblas PATHS ENV LIBRARY_PATH)
find_library(LAPACK_LIBRARIES NAMES lapack coinlapack PATHS ENV LIBRARY_PATH)
find_library(MUMPS_LIBRARIES NAMES coinmumps PATHS ENV LIBRARY_PATH)
add_library(PowerTools++ ${SOURCES} ${HEADERS})
target_link_libraries(PowerTools++ ${GUROBI_LIBRARIES} ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} ${IPOPT_LIBRARIES} dl coinhsl coinmetis ${MUMPS_LIBRARIES})


add_executable(PowerTools main.cpp)
target_link_libraries(PowerTools PowerTools++)

install(TARGETS PowerTools++
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
