cmake_minimum_required(VERSION 2.8.3)
project(neat_gru_bug)

add_subdirectory(lib/NEAT_GRU)
add_subdirectory(src/argos)

# Set ARGoS link dir
link_directories(${ARGOS_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} src/main.cpp src/ga.cpp)

target_link_libraries(neat_gru_bug
   neatGRULib
   argos_simulation
   )

set(EXT_LIB_INCLUDES
   ${CMAKE_SOURCE_DIR}/lib/)

target_include_directories(neat_gru_bug PUBLIC
   ${EXT_LIB_INCLUDES})
