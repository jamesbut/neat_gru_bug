cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
#It seems you need these for boost to work
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)

project(neat_gru_bug)

add_subdirectory(lib/NEAT_GRU)
add_subdirectory(src/argos)

# Set ARGoS link dir
link_directories(${ARGOS_LIBRARY_DIRS})

#find_package(GUROBI REQUIRED)

#Find boost
#I can't seem to get 1.68.0 working at the moment,
#this works for now
find_package (Boost 1.67.0 REQUIRED COMPONENTS system filesystem)

add_executable(${PROJECT_NAME} src/main.cpp src/ga.cpp src/individual_run.cpp
                               src/data_collection.cpp src/shared_memory.cpp
                               src/nash_averaging.cpp)


target_link_libraries(${PROJECT_NAME}
   neatGRULib
   argos_simulation
   rt
   ${Boost_LIBRARIES}
   #/opt/gurobi810/linux64/src/build/libgurobi_c++.a
   #/opt/gurobi810/linux64/lib/libgurobi81.so
   /usr/lib/python2.7/config-x86_64-linux-gnu/libpython2.7.so
   )

set(EXT_LIB_INCLUDES
   ${CMAKE_SOURCE_DIR}/lib/)

target_include_directories(${PROJECT_NAME} PUBLIC
   ${EXT_LIB_INCLUDES}
   #/opt/gurobi810/linux64/include/
   )
