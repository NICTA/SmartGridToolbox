include_directories(${PROJECT_INCLUDE_DIR})

file(GLOB HEADERS ${PROJECT_INCLUDE_DIR} *.h)
file(GLOB SOURCES . *.cpp ${PROJECT_INCLUDE_DIR}/PowerTools++/*.h)

add_library(PowerTools++ ${SOURCES} ${HEADERS})
target_link_libraries(PowerTools++ ${GUROBI_LIBRARIES} ${IPOPT_LIBRARIES})

add_executable(PowerTools main.cpp)
target_link_libraries(PowerTools PowerTools++)
