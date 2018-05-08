set(EXT_LIB_INCLUDES
    ${CMAKE_SOURCE_DIR}/lib/)

add_library(neat_gru_footbot_controller MODULE neat_gru_footbot.cpp)

target_link_libraries(neat_gru_footbot_controller
  argos3core_simulator
  argos3plugin_simulator_footbot
  argos3plugin_simulator_genericrobot)

target_include_directories(neat_gru_footbot_controller PUBLIC
   ${EXT_LIB_INCLUDES}
   )
