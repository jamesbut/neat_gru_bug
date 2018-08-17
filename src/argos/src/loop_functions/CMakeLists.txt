add_library(loop_functions_lib SHARED master_loop_function.cpp fitness/fitness_score.cpp
            kim_environment_generator.cpp fitness/trajectory_logger.cpp fitness/astar_on_env.cpp
            fitness/search_grid_wrapper.cpp)

set(EXT_LIB_INCLUDES
   ${CMAKE_SOURCE_DIR}/lib/)

#message(STATUS ${aiTools_includes})

target_include_directories(loop_functions_lib PUBLIC
   ${EXT_LIB_INCLUDES}
   )

find_package( OpenCV REQUIRED )

target_link_libraries(loop_functions_lib
   neat_gru_footbot_controller
   ${OpenCV_LIBS}
   )
