add_library(loop_functions SHARED master_loop_function.cpp fitness_score.cpp kim_environment_generator.cpp trajectory_logger.cpp)

set(EXT_LIB_INCLUDES
   ${CMAKE_SOURCE_DIR}/lib/)

target_include_directories(loop_functions PUBLIC
   ${EXT_LIB_INCLUDES})

find_package( OpenCV REQUIRED )

target_link_libraries(loop_functions
   neat_gru_footbot_controller
   ${OpenCV_LIBS}
   )
