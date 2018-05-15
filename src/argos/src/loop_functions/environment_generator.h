#ifndef ENVIRONMENT_GENERATOR_H_
#define ENVIRONMENT_GENERATOR_H_

#include <argos3/core/simulator/loop_functions.h>

#include <opencv2/opencv.hpp>

using namespace argos;

class EnvironmentGenerator {

public:

   EnvironmentGenerator();
   ~EnvironmentGenerator();

   void Init();
   void Reset(int env_num);

private:

   void clear_environment();

   void generate_env_from_file(std::string file_name);
   void generate_random_env();

   void put_blocks_in_environment();
   void put_lines_in_environment();

   CVector3 arena_size;

   const bool RANDOMLY_GENERATED;
   const std::string FILENAME_PREFIX;
   const bool BLOCKED_ENV;

   cv::Mat read_img;

};

#endif
