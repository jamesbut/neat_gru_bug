add_library(master_loop_function SHARED master_loop_function.cpp)

set(EXT_LIB_INCLUDES
   ${CMAKE_SOURCE_DIR}/lib/)

target_include_directories(master_loop_function PUBLIC
   ${EXT_LIB_INCLUDES})

target_link_libraries(master_loop_function
   neat_gru_footbot_controller
   )
