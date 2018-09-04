#ifndef _ENVIRONMENT_GENERATOR_H_
#define _ENVIRONMENT_GENERATOR_H_

#include <string>

#include <argos3/core/utility/math/vector2.h>

//OpenCV libraries
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

/*
   This class is in charge of generating environments and storing information
   about them such as shortest path from astar search.

   This way, this can all be done outside the parallel processes, because currently
   each process does search which is highly inefficient.
*/

struct grid_element_status_t {
  bool is_agent_present;
  std::vector<std::vector<int>> circ_action;
  bool is_corridor_present;
};

class EnvironmentGenerator {

public:

   EnvironmentGenerator(const std::string filename);

   void generate_env();

   inline cv::Mat get_environment() {return corridor_contours_img;};
   inline double get_environment_optimal_length() {return optimal_path_length;};
   inline int get_env_width() {return environment_width;};
   inline int get_env_height() {return environment_height;};

   const bool EFFICIENT_ENVIRONMENT;

private:

   void read_file();
   void generate_rand_env();
   void calculate_optimal_path_length();

   //Kims functions
   bool checkConnectivity();
   void getRobotPositions();
   void initializeGrid();
   void initializeAgents();
   void findAgents();
   void decideNextAction(std::vector<int> current_bot_position);
   void setNextLocation(std::vector<int> current_bot_position);
   float getCorridorPercentage();
   void makeBinaryImageCorridors();
   void makeBoundariesCorridors();
   void makeRooms();
   void makeRandomOpenings();
   void dfs(int x, int y, int current_label);
   int mod(int a, int b);

   const std::string FILE_NAME;
   const float WANTED_CORRIDOR_PERCENTAGE;
   const float CHANGE_AGENT_GOSTRAIGHT;
   const float ROOM_PERCENTAGE;
   const int AMOUNT_OF_OPENINGS;

   //Imaage related variables
   cv::Mat bin_corridor_img;
   cv::Mat bin_corridor_img_large;
   cv::Mat corridor_contours_img;
   cv::Mat corridor_contours_img_save;
   int environment_width;
   int environment_height;

   std::vector<std::vector<int>> initial_bot_positions;
   std::vector<std::vector<int>> current_agent_positions;

   std::vector<std::vector<grid_element_status_t>> environment_grid;

   std::vector<std::vector<int>> connectivity_labels;

   cv::RNG rng;

   //Search results
   std::vector<argos::CVector2> optimal_path;
   double optimal_path_length;


};

#endif
