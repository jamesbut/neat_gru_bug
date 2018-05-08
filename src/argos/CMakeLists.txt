cmake_minimum_required(VERSION 2.8.12)

# Deactivate RPATH for MacOSX
set(CMAKE_MACOSX_RPATH 0)

# Set the path where CMake will find additional scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Find the ARGoS package, make sure to save the ARGoS prefix
find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)
set(ARGOS_PREFIX ${ARGOS_PREFIX} CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

# Check whether all the necessary libs have been installed to compile the
# code that depends on Qt and OpenGL
include(ARGoSCheckQTOpenGL)

# Find Lua
find_package(Lua52 REQUIRED)

# Set ARGoS include dir
include_directories(${catkin_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS} ${LUA_INCLUDE_DIR})

# Set ARGoS link dir
link_directories(${ARGOS_LIBRARY_DIRS})

add_subdirectory(src/controllers)
#add_subdirectory(src/loop_functions)

add_library(argos_simulation SHARED src/argos_simulation.cpp)

target_link_libraries(argos_simulation
  argos3core_simulator
  )
