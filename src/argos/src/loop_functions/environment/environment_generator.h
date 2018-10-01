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

   EnvironmentGenerator();
   ~EnvironmentGenerator();

   void generate_env(const std::string filename, const int env_num);

   inline cv::Mat get_environment() {return corridor_contours_img;};
   inline double get_environment_optimal_length() {return optimal_path_length;};
   inline int get_env_width_divide_two() {return environment_width;};
   inline int get_env_height_divide_two() {return environment_height;};
   inline int get_env_width() {return 2 * environment_width;};
   inline int get_env_height() {return 2 * environment_height;};

   void set_argos_config_file(const std::string filename);

   double calculate_remaining_distance_from(const argos::CVector2 pos);

   const bool EFFICIENT_ENVIRONMENT;

private:

   void read_file(const std::string file_name);
   void generate_rand_env();
   //void calculate_optimal_path_length();
   double calculate_path_length(const std::vector<argos::CVector2> path);

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

   //const std::string FILE_NAME;
   const float WANTED_CORRIDOR_PERCENTAGE;
   const float CHANGE_AGENT_GOSTRAIGHT;
   const float ROOM_PERCENTAGE;
   const int AMOUNT_OF_OPENINGS;

   const std::string TEST_ENV_LENGTHS_PATH;

   //Imaage related variables
   cv::Mat bin_corridor_img;
   cv::Mat bin_corridor_img_large;
   cv::Mat corridor_contours_img;
   cv::Mat corridor_contours_img_save;

   cv::Mat corridor_contours_img_stored;

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

double get_value_at_line(std::string file_name, unsigned int num);

#endif
