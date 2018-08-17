cmake_minimum_required(VERSION 2.8.12)

PROJECT( neat_gru )

include_directories(include)

FILE(GLOB neat_sources src/*.cpp)

add_library(neatGRULib SHARED ${neat_sources})
