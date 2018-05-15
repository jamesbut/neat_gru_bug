#include "environment_generator.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <argos3/plugins/simulator/entities/box_entity.h>

using namespace cv;

EnvironmentGenerator::EnvironmentGenerator() :
   RANDOMLY_GENERATED(false),
   FILENAME_PREFIX("../argos_params/environments/kims_envs/rand_env_"),
   BLOCKED_ENV(true) {}

EnvironmentGenerator::~EnvironmentGenerator() {}

void EnvironmentGenerator::Init() {

   arena_size = CSimulator::GetInstance().GetSpace().GetArenaSize();

   std::cout << arena_size << std::endl;

   // int environment_width = (int)(arena_size.GetX()/2);
   // int environment_height=(int)(arena_size.GetY()/2);
   //
   // std::cout << environment_width << std::endl;
   // std::cout << environment_height << std::endl;

}

void EnvironmentGenerator::Reset(int env_num) {

   std::cout << "Reseting the environment" << std::endl;

   //Create file name
   std::string file_name = FILENAME_PREFIX + std::to_string(env_num+1) + ".png";

   if (RANDOMLY_GENERATED) generate_random_env();
   else generate_env_from_file(file_name);

}

void EnvironmentGenerator::generate_random_env() {}

void EnvironmentGenerator::generate_env_from_file(std::string file_name) {

   //Read file
   read_img = imread(file_name, CV_LOAD_IMAGE_GRAYSCALE);

   if (!read_img.data) {

      std::cout << "Could not open environment file" << std::endl;
      exit(1);

   }

   //std::cout << read_img << std::endl;

   //imshow("Environment image", read_img);

   if(BLOCKED_ENV) put_blocks_in_environment();
   else put_lines_in_environment();

   //waitKey(0);

}

void EnvironmentGenerator::put_blocks_in_environment() {

   CLoopFunctions loopfunction;

   CVector3 boxEntitySize {0.1, 0.1, 0.5};
   CQuaternion boxEntityRot {0, 0, 0, 0};

   std::cout << read_img.rows << std::endl;
   std::cout << read_img.cols << std::endl;

   for(int i = 0; i < read_img.rows; i++) {
      for(int j = 0; j < read_img.cols; j++) {
         if(read_img.at<uchar>(i, j) == 255) {

            std::string boxName = "box" + std::to_string(i * read_img.cols + j);
            std::cout << boxName << std::endl;
            double x_pos = (double)(j / 10 - (arena_size.GetX()/2));
            double y_pos = (double)(i / 10 - (arena_size.GetY()/2));
            std::cout << x_pos << " " << y_pos << std::endl;
            CVector3 boxEntityPos{x_pos, y_pos, 0};
            CBoxEntity* boxEntity = new CBoxEntity(boxName, boxEntityPos, boxEntityRot, false, boxEntitySize);

            loopfunction.AddEntity(*boxEntity);

         }
      }
   }

}

void EnvironmentGenerator::put_lines_in_environment() {

   //TODO: Come back to line detection in future

   Mat canny_lined_img, hough_lined_img;

   //Canny edge detection
   Canny(read_img, canny_lined_img, 50, 200, 3);

   imshow("Lined image", canny_lined_img);

   std::vector<Vec4i> lines;
   HoughLinesP(read_img, lines, 1, CV_PI/180, 50, 50, 10 );

   for( size_t i = 0; i < lines.size(); i++ ) {
      Vec4i l = lines[i];
      line( read_img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
   }

   imshow("Hough lines", read_img);

}

void EnvironmentGenerator::clear_environment() {

   // Just used to clear entities
   CLoopFunctions loopfunction;

   std::cout << "Clearing environment" << std::endl;

   //Might be able to get away with just removing by boxEntities.size() here

}
