#cmake_minimum_required(VERSION 2.8.3)
cmake_minimum_required(VERSION 3.5)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "-Wreorder")
project(neat_gru_bug)

add_subdirectory(lib/NEAT_GRU)
add_subdirectory(src/argos)

# Set ARGoS link dir
link_directories(${ARGOS_LIBRARY_DIRS})

#Find boost
find_package (Boost 1.55.0 REQUIRED COMPONENTS filesystem)

add_executable(${PROJECT_NAME} src/main.cpp src/ga.cpp src/individual_run.cpp
                               src/data_collection.cpp)

target_link_libraries(neat_gru_bug
   neatGRULib
   argos_simulation
   rt
   ${Boost_LIBRARIES}
   )

set(EXT_LIB_INCLUDES
   ${CMAKE_SOURCE_DIR}/lib/)

target_include_directories(neat_gru_bug PUBLIC
   ${EXT_LIB_INCLUDES})
