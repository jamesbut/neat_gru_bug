#include "environment_builder.h"

//OpenCV libraries
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//ARGoS libraries
#include <argos3/core/simulator/loop_functions.h>

EnvironmentBuilder::EnvironmentBuilder() : total_boxes_generated(0),
                                           it_box(0) {}

void EnvironmentBuilder::build_env(EnvironmentGenerator& env_generator) {

   //Clear environment
   clearEnvironment();
   it_box = 0;

   //Build new environment
   if (env_generator.EFFICIENT_ENVIRONMENT)
      putLinesInEnvironment(env_generator);      //Comment out for handwritten envs

   putBlocksInEnvironment(env_generator);

   //std::cout << "Env builder has built!" << std::endl;

}

void EnvironmentBuilder::putLinesInEnvironment(EnvironmentGenerator& env_generator) {

   //Mat dst;
   //Canny(corridor_contours_img, dst, 50, 200, 3);     //This never used to be here

  // Show our image inside it.
  std::vector<cv::Vec4i> lines;
  //Check to see whether map is randomly generated or taken from .png
  //this affects how the walls scale
  if(_map_request_type == 3) {
     HoughLinesP(env_generator.get_environment(), lines, 1, CV_PI/180*45, 18, 20, 5 );     //New new
     //HoughLinesP(corridor_contours_img, lines, 1, CV_PI/180*45, 20, 0, 5 );      //Old version
     //HoughLinesP(corridor_contours_img, lines, 1, CV_PI/180*45, 18, 0, 5);     //New
  } else {
     HoughLinesP(env_generator.get_environment(), lines, 1, CV_PI/180*90, 10, 0, 0);
 }
  // namedWindow( "corridor_contours_img", WINDOW_AUTOSIZE );
  // imshow( "corridor_contours_img", corridor_contours_img );
  // namedWindow( "img_lines ", WINDOW_AUTOSIZE );

  //Show the hough detection
  cv::Mat img_lines = env_generator.get_environment().clone();
  for( size_t i = 0; i < lines.size(); i++ )
  {

    cv::Vec4i l = lines[i];
    line( img_lines, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(100,100,100), 3, CV_AA);       //original
    line(env_generator.get_environment(), cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,0), 1, CV_AA);
    //line(dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,0), 2, CV_AA);


  }

  //imshow( "img_lines ", corridor_contours_img);
  //waitKey(0);

  // Initialize box entity characteristics
  argos::CBoxEntity* boxEntity;
  argos::CQuaternion boxEntityRot{0, 0, 0, 0};
  argos::CVector3 boxEntitySize{0.1, 0.1, 0.5};
  std::ostringstream box_name;

  argos::CLoopFunctions loopfunction;
  for( size_t i = 0; i < lines.size(); i++ )
  {
    // Transform the hough line coordinates to argos coordinates
    cv::Vec4i l = lines[i];
    std::vector<double> argos_coordinates{(double)((l[1]+l[3])/2 - env_generator.get_env_width() * 20 / 2) / 10.0f, (double)((l[0]+l[2])/2 - env_generator.get_env_height() *20/ 2) / 10.0f};
    argos::CVector3 boxEntityPos{argos_coordinates.at(0), argos_coordinates.at(1), 0};
    double box_lenght = (sqrt(pow((double)(l[2]-l[0]),2.0f)+pow((double)(l[3]-l[1]),2.0f))+2)/10.0f;
    //double box_lenght = (sqrt(pow((double)(l[2]-l[0]),2.0f)+pow((double)(l[3]-l[1]),2.0f))+4)/10.0f;
    //Check to see whether map is randomly generated or taken from .png
    //this affects how the walls scale
    if(_map_request_type == 3) {
      //boxEntitySize.Set(box_lenght,0.2,0.5);
      boxEntitySize.Set(box_lenght,0.1,0.5);
   } else {
      boxEntitySize.Set(box_lenght,0.2,0.5);
   }
    const argos::CRadians orientation = (argos::CRadians)(atan2(l[2]-l[0],l[3]-l[1]));
    const argos::CRadians zero_angle = (argos::CRadians)0;
    boxEntityRot.FromEulerAngles(orientation,zero_angle,zero_angle);

    // Set entity in environment
    box_name.str("");
    box_name << "box" << (it_box);
    boxEntity = new argos::CBoxEntity(box_name.str(), boxEntityPos, boxEntityRot, false, boxEntitySize);
    loopfunction.AddEntity(*boxEntity);

    // Save the box entities to be accurately removed with reset
    boxEntities.push_back(boxEntity);
    it_box++;

  }

  total_boxes_generated=it_box-1;

}

void EnvironmentBuilder::putBlocksInEnvironment(EnvironmentGenerator& env_generator) {

   argos::CBoxEntity* boxEntity;
   argos::CVector3 boxEntitySize;
   if (env_generator.EFFICIENT_ENVIRONMENT)
      //CVector3 boxEntitySize{0.3, 0.3, 0.5};
      //CVector3 boxEntitySize{0.1, 0.1, 0.5};
      boxEntitySize = argos::CVector3{0.2, 0.2, 0.5};
   else
      boxEntitySize = argos::CVector3{0.1, 0.1, 0.5};

   argos::CQuaternion boxEntityRot{0, 0, 0, 0};

   std::ostringstream box_name;

 /*  if(i<total_boxes_generated)
   {
     loop_function.MoveEntity(boxEntities.at(i)->GetEmbodiedEntity(),boxEntityPos,boxEntityRot);
   }else
   {
     CBoxEntity *boxEntity = new CBoxEntity(box_name.str(), boxEntityPos, boxEntityRot, false, boxEntitySize);
     loop_function.AddEntity(*boxEntity);
     boxEntities.push_back(boxEntity);
   }*/

   argos::CLoopFunctions loopfunction;
   for (int itx = 0; itx < env_generator.get_env_width() * 20; itx++) {
     for (int ity = 0; ity < env_generator.get_env_height() * 20; ity++) {
       if (env_generator.get_environment().at<uchar>(ity, itx) == 255) {
         box_name.str("");
         box_name << "box" << (it_box);
         std::vector<double> argos_coordinates{(double)(itx - env_generator.get_env_width() * 10) / 10.0f, (double)(ity - env_generator.get_env_height() * 10) / 10.0f};
         argos::CVector3 boxEntityPos{argos_coordinates.at(1), argos_coordinates.at(0), 0};
         boxEntity = new argos::CBoxEntity(box_name.str(), boxEntityPos, boxEntityRot, false, boxEntitySize);

         loopfunction.AddEntity(*boxEntity);

         boxEntities.push_back(boxEntity);


         it_box++;
       }

     }
   }
   total_boxes_generated=it_box-1;

}

void EnvironmentBuilder::clearEnvironment() {

   //std::cout<<"Clear environment"<<std::endl;
   argos::CLoopFunctions loopfunction;

   if(total_boxes_generated!=0)
   {
     for(int i = 0;i<total_boxes_generated+1;i++){
       //auto start_time = std::chrono::high_resolution_clock::now();

       loopfunction.RemoveEntity(*boxEntities.at(i));
       /* auto end_time = std::chrono::high_resolution_clock::now();
      auto time = end_time - start_time;

      std::cout << "It took " <<
        std::chrono::duration_cast<std::chrono::microseconds>(time).count() << " to run.\n";*/
     }
   }
      boxEntities.clear();
      total_boxes_generated =0;

}
