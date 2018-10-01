#include "environment_generator.h"

#include "../fitness/astar_on_env.h"

#include <argos3/core/utility/configuration/argos_configuration.h>

using namespace cv;
//using namespace argos;

//#define ACCEPT_ENVIRONMENT

EnvironmentGenerator::EnvironmentGenerator() :
   WANTED_CORRIDOR_PERCENTAGE(0.4f),
   CHANGE_AGENT_GOSTRAIGHT(0.7f),
   ROOM_PERCENTAGE(0.4f),
   AMOUNT_OF_OPENINGS(11),
   EFFICIENT_ENVIRONMENT(true),
   TEST_ENV_LENGTHS_PATH("../trajectories_temp/kims_envs_lengths/kims_envs_lengths.txt"),
   //rng(5)
   rng(rand())
   {

      //std::srand(5);
      std::srand(rand());

      //TODO: Would be nice to get this from somewhere else
      //I can probably get it from the xml file using the argos code
      //It's quite complicated, I will have to donwload the tinyXML library
      //and use that, but it can be done.
      // environment_width = 14;
      // environment_height = 14;

      environment_width = 14/2;
      environment_height = 14/2;

      //Just to jig rng to same place as kims version
      rng.uniform(0.0f,1.0f);

   }

EnvironmentGenerator::~EnvironmentGenerator() {

   //std::cout << "Env generator destroyed" << std::endl;

}

//TODO: Come back to this eventually, not essential for now
//This function also overides the environment height and width
//by reading the xml file
void EnvironmentGenerator::set_argos_config_file(const std::string filename) {

   //argos::tcipp::Document doc = tcipp::Document();
   //doc.LoadFile(filename);

   //environment_width =
   //environment_height =

}

void EnvironmentGenerator::generate_env(const std::string filename, const int env_num) {

   //corridor_contours_img_stored

   //This means the environment has an image already, for example in the test data
   if(filename != "") {

      //Read in image
      read_file(filename);

      optimal_path_length = get_value_at_line(TEST_ENV_LENGTHS_PATH, env_num);

   //Otherwise generate a random env
   } else {

      //Generate random environment
      generate_rand_env();

      //Call astar here
      optimal_path = astar_on_env(corridor_contours_img, argos::CVector2(20, 20), argos::CVector2(110, 110));

      //Calculate path length too
      optimal_path_length = calculate_path_length(optimal_path);

   }

}


double EnvironmentGenerator::calculate_path_length(const std::vector<argos::CVector2> path) {

   std::vector<argos::CVector2> astar_path_divided_by_ten(path.size());

   //Divide astar path by 10 because the image that is planned over is 140x140
   //whereas the environment is 14x14
   std::transform(path.begin(), path.end(), astar_path_divided_by_ten.begin(),
                  [](const argos::CVector2& pos){return pos / 10;});

   double length = 0.0;
   //std::cout << "Op path size: " << path.size() << std::endl;

   for(size_t i = 0; i < path.size()-1; i++) {
      //std::cout << "i: " << i << std::endl;
      double x_diff = std::abs(astar_path_divided_by_ten[i].GetX() - astar_path_divided_by_ten[i+1].GetX());
      double y_diff = std::abs(astar_path_divided_by_ten[i].GetY() - astar_path_divided_by_ten[i+1].GetY());

      length += sqrt(pow(x_diff, 2) + pow(y_diff, 2));

   }

   return length;

}

double EnvironmentGenerator::calculate_remaining_distance_from(const argos::CVector2 pos) {

   std::vector<argos::CVector2> remaining_path = astar_on_env(corridor_contours_img_stored, pos, argos::CVector2(110, 110));

   return calculate_path_length(remaining_path);

}

void EnvironmentGenerator::generate_rand_env() {

   bool environment_accepted = false;
   bool corridors_are_connected = false;

   while(!environment_accepted){
     while (!corridors_are_connected) {
       getRobotPositions();
       initializeGrid();
       initializeAgents();
       bin_corridor_img = Mat::zeros(environment_width, environment_height, CV_8UC1);

       for (int it_total = 0; it_total < 100; it_total++) {

         findAgents();
         //std::cout << "-------" << std::endl;

         for (int it = 0; it < current_agent_positions.size(); it++) {
            //std::cout << it << std::endl;
           decideNextAction(current_agent_positions.at(it));    //Randomness in here //This is called a random num of times
           setNextLocation(current_agent_positions.at(it));

           //std::cout << "-----" << std::endl;

         }

         //std::cout << "*********" << std::endl;

 /*        for (int itx = 0; itx < environment_width; itx++) {
            for (int ity = 0; ity < environment_height; ity++) {
                cout<<environment_grid.at(itx).at(ity).is_corridor_present<<" ";
            }
            cout<<" "<<endl;
          }*/
          //std::cout << it_total << std::endl;
          //std::cout << "Corridor perc: " << getCorridorPercentage() << std::endl;
         if (getCorridorPercentage() > WANTED_CORRIDOR_PERCENTAGE) {
           break;
         }


       }

       //std::cout << "........................................" << std::endl;

       corridors_are_connected = checkConnectivity();

       //James - does she seed the rng with a tick count?
       //I think the rng does not have to be reinitilised here
       //it will just move onto the next random number
       if(!corridors_are_connected) {
         //std::cout<<"corridors are not connected!!"<<std::endl;
         //rng = cv::getTickCount();
       }

     }

     makeBinaryImageCorridors();
     makeBoundariesCorridors();
     makeRooms();
     makeRandomOpenings();     //Randomness in here too

     cv::Rect border(cv::Point(0, 0), corridor_contours_img.size());

     rectangle(corridor_contours_img, border, Scalar(255), 3);

     //Write to file
   //   std::stringstream file_name;
   //   file_name << GENERATED_ENVS_MAP_PATH << env_num << ".png";
     //
   //   //Write images of generated envs
   //   cv::imwrite(file_name.str(),corridor_contours_img);

   // std::cout << "First" << std::endl;
   // std::cout << corridor_contours_img.rows << " " << corridor_contours_img.cols << std::endl;

     #ifdef ACCEPT_ENVIRONMENT

         namedWindow( "Environment2", WINDOW_AUTOSIZE );// Create a window for display.
         imshow( "Environment2", corridor_contours_img );                   // Show our image inside it.
         char key = (char)waitKey(0);
         if(key=='y')
         {
           environment_accepted=true;

           cvDestroyWindow("Environment2");
           //cvReleaseImage(&images[i]);

           break;
         }else
         {
           //rng = cv::getTickCount();
           corridors_are_connected = false;
         }
     #else
         environment_accepted=true;
     #endif

  }

  corridor_contours_img_stored = corridor_contours_img.clone();

}

void EnvironmentGenerator::read_file(const std::string file_name) {

   //std::cout << "filename: " << file_name << std::endl;

   cv::Mat read_img = cv::imread(file_name, CV_LOAD_IMAGE_GRAYSCALE);

   corridor_contours_img = Mat::zeros(read_img.size(), CV_8UC1);
   resize(read_img, corridor_contours_img, corridor_contours_img.size(), 0, 0, INTER_NEAREST);

   corridor_contours_img_stored = corridor_contours_img.clone();

}

bool EnvironmentGenerator::checkConnectivity() {

   std::vector<int> linked;
   //Resizing connectivity_labels grid
   connectivity_labels.resize(environment_width);
   for (int it = 0; it < environment_width; it++) {
     connectivity_labels[it].resize(environment_height);
   }

   for (int itx = 0; itx < environment_width; itx++) {
     for (int ity = 0; ity < environment_height; ity++) {
       connectivity_labels.at(itx).at(ity)=0;
     }
   }
   std::vector<int> neighbors_labels;
   int next_label = 0;

   int component = 0;
   for (int itx = 0; itx < environment_width; itx++) {
     for (int ity = 0; ity < environment_height; ity++) {
       if (!connectivity_labels.at(itx).at(ity)&&environment_grid.at(itx).at(ity).is_corridor_present)
         {
         dfs(itx, ity, ++component);
         }
     }
   }

 /*
   for (int itx = 0; itx < environment_width; itx++) {
      for (int ity = 0; ity < environment_height; ity++) {
          cout<<connectivity_labels.at(itx).at(ity)<<" ";
      }
      cout<<" "<<endl;
    }
 */

   int label_at_first_location = connectivity_labels.at(initial_bot_positions.at(0).at(1)).at(initial_bot_positions.at(0).at(0));

   for (int it = 1; it < initial_bot_positions.size(); it++) {
     int label_at_second_location = connectivity_labels.at(initial_bot_positions.at(it).at(1)).at(initial_bot_positions.at(it).at(0));
     if (label_at_first_location == label_at_second_location) {
       return true;
     } else {
       return false;
       break;
     }
   }
}

void EnvironmentGenerator::getRobotPositions() {

   //TODO: NEED TO HAVE START AND GOAL NOT DUPLICATED HERE REALLY
   //TODO: Might use the env_width and env_heights again at some point

   std::vector<int> initial_bot_position_start {0, 0};
   std::vector<int> initial_bot_position_goal {5, 5};

   // std::cout << "Initial position: start: " << initial_bot_position_start[0] << " " << initial_bot_position_start[1]
   // << " goal: " << initial_bot_position_goal[0] << " " << initial_bot_position_goal[1] << std::endl;


   initial_bot_positions.push_back(initial_bot_position_start);
   initial_bot_positions.push_back(initial_bot_position_goal);

   // CSpace::TMapPerType& tFBMap = CSimulator::GetInstance().GetSpace().GetEntitiesByType("foot-bot");
   //
   // for(CSpace::TMapPerType::iterator it = tFBMap.begin();
   //     it != tFBMap.end();
   //     ++it) {
   //
   //    CFootBotEntity* pcFB = any_cast<CFootBotEntity*>(it->second);
   //    CVector3 pos_bot;
   //    pos_bot = pcFB->GetEmbodiedEntity().GetOriginAnchor().Position;
   //    vector<int> initial_bot_position{0,0};
   //    initial_bot_position.at(0)=pos_bot.GetX()/2+environment_width/2;
   //    initial_bot_position.at(1)=pos_bot.GetY()/2+environment_height/2;
   //    initial_bot_positions.push_back(initial_bot_position);
   // }

}

void EnvironmentGenerator::initializeGrid() {

  std::vector<std::vector<int>> circ_action_init{{0, 0}, {0, 0}, {0, 0}, {0, 0}};
  //Resizing environment grid
  environment_grid.resize(environment_width);
  for (int it = 0; it < environment_width; it++) {
    environment_grid[it].resize(environment_height);
  }

  //TODO: get this like trajectory_loop_function does
  for (int itx = 0; itx < environment_width; itx++) {
    for (int ity = 0; ity < environment_height; ity++) {
      environment_grid.at(itx).at(ity).is_corridor_present = false;
      environment_grid.at(itx).at(ity).is_agent_present = false;
      environment_grid.at(itx).at(ity).circ_action = circ_action_init;
    }
  }

}

void EnvironmentGenerator::initializeAgents() {

  current_agent_positions.resize(2);
  // initial robot positions, place agent where they are
  std::vector<std::vector<int>> circ_action_init{{0, 1}, {1, 0}, {0, -1}, { -1, 0}};


  for (int it = 0; it < initial_bot_positions.size(); it++) {
    environment_grid.at(initial_bot_positions.at(it).at(1)).at(initial_bot_positions.at(it).at(0)).is_agent_present = true;

    std::rotate(circ_action_init.begin(), circ_action_init.begin() + std::rand()%4, circ_action_init.end());
    environment_grid.at(initial_bot_positions.at(it).at(1)).at(initial_bot_positions.at(it).at(0)).circ_action = circ_action_init;

  }

}

void EnvironmentGenerator::findAgents() {

  current_agent_positions.clear();

  int k = 0;
  for (int itx = 0; itx < environment_width; itx++) {
    for (int ity = 0; ity < environment_height; ity++) {
      if (environment_grid.at(itx).at(ity).is_agent_present) {
        current_agent_positions.resize(k + 1);
        current_agent_positions.at(k).resize(2);
        current_agent_positions.at(k).at(0) = itx;
        current_agent_positions.at(k).at(1) = ity;
        k++;
      }
    }
  }

 //  for (int i = 0; i < current_agent_positions.size(); i++) {
 //     for(int j = 0; j < current_agent_positions[i].size(); j++) {
 //        std::cout << current_agent_positions[i][j] << " ";
 //     }
 //     std::cout << std::endl;
 // }


}

int EnvironmentGenerator::mod(int a, int b)
{ return (a % b + b) % b; }

void EnvironmentGenerator::decideNextAction(std::vector<int> current_bot_position) {

  float random_percentage = rng.uniform(0.0f,1.0f);
  //std::cout << "Agent at: " << current_bot_position[0] << ", " << current_bot_position[1] << std::endl;
  //std::cout << "RNG state: " << rng.state << std::endl;
  //std::cout << "Random percentage: " << random_percentage << std::endl;
  float percentage_rest = 1.0f - CHANGE_AGENT_GOSTRAIGHT;

  //Analogous to..
  //So this just gets the circ_action for the environment grid that the agent is at
  //environment_grid[current_bot_position[0]][current_bot_position[1]].circ_action
  std::vector<std::vector<int>>circ_action_temp = environment_grid.at(current_bot_position.at(0)).at(current_bot_position.at(1)).circ_action;

 //  std::cout << "Circ action temp:" << std::endl;
 //  for(int i = 0; i < circ_action_temp.size(); i++) {
 //     for(int j = 0; j < circ_action_temp[i].size(); j++) {
 //        std::cout << circ_action_temp[i][j] << " ";
 //     }
 //     std::cout << std::endl;
 // }

  //string state;
  if (random_percentage <= CHANGE_AGENT_GOSTRAIGHT) {
    //std::cout << "STRAIGHT" << std::endl;
    //state = "GO_STRAIGHT";
  } else if (random_percentage > CHANGE_AGENT_GOSTRAIGHT &&
             random_percentage <= CHANGE_AGENT_GOSTRAIGHT + percentage_rest / 2.0f) {
    //std::cout << "LEFT" << std::endl;
    //state = "GO_LEFT";
    std::rotate(circ_action_temp.begin(), circ_action_temp.begin() + 1, circ_action_temp.end());

  } else if (random_percentage > CHANGE_AGENT_GOSTRAIGHT + percentage_rest / 2.0f &&
             random_percentage <= 1) {
    //std::cout << "RIGHT" << std::endl;
    //state = "GO_RIGHT";
    std::rotate(circ_action_temp.rbegin(), circ_action_temp.rbegin() + 1, circ_action_temp.rend());
  }

  environment_grid.at(current_bot_position.at(0)).at(current_bot_position.at(1)).circ_action = circ_action_temp;

}

void EnvironmentGenerator::setNextLocation(std::vector<int> current_bot_position) {

  std::vector<std::vector<int>>circ_action_temp = environment_grid.at(current_bot_position.at(0)).at(current_bot_position.at(1)).circ_action;
  std::vector<int> next_location{current_bot_position.at(0) + circ_action_temp.at(0).at(0), current_bot_position.at(1) + circ_action_temp.at(0).at(1)};


  std::vector<int> next_location_corrected{mod(next_location.at(0), environment_width), mod(next_location.at(1), environment_height)};

  environment_grid.at(current_bot_position.at(0)).at(current_bot_position.at(1)).is_agent_present = false;
  environment_grid.at(current_bot_position.at(0)).at(current_bot_position.at(1)).is_corridor_present = true;
  environment_grid.at(next_location_corrected.at(0)).at(next_location_corrected.at(1)).is_agent_present = true;
  environment_grid.at(next_location_corrected.at(0)).at(next_location_corrected.at(1)).circ_action = circ_action_temp;

}

float EnvironmentGenerator::getCorridorPercentage() {

  int count_corridor = 0;
  for (int itx = 0; itx < environment_width; itx++) {
    for (int ity = 0; ity < environment_height; ity++) {
      if (environment_grid.at(itx).at(ity).is_corridor_present) {
        count_corridor++;
      }
    }
  }

  return (float)count_corridor / (float)(environment_width * environment_height);

}

void EnvironmentGenerator::makeBinaryImageCorridors() {

  for (int itx = 0; itx < environment_width; itx++) {
    for (int ity = 0; ity < environment_height; ity++) {
      // cout << environment_grid.at(itx).at(ity).is_corridor_present << " ";
      if (environment_grid.at(itx).at(ity).is_corridor_present) {
        bin_corridor_img.at<uchar>(itx, ity) = 255;
      }
    }
  }

}

void EnvironmentGenerator::makeBoundariesCorridors() {
  bin_corridor_img_large = Mat::zeros(environment_width * 20, environment_height * 20, CV_8UC1);
  resize(bin_corridor_img, bin_corridor_img_large, bin_corridor_img_large.size(), 0, 0, INTER_NEAREST);

  dilate(bin_corridor_img_large, bin_corridor_img_large, Mat(), Point(-1, -1), 2, 1, 1);
  dilate(bin_corridor_img_large, bin_corridor_img_large, Mat(), Point(-1, -1), 2, 1, 1);

  for(int it =0; it<initial_bot_positions.size();it++)
    rectangle(bin_corridor_img_large, Point(initial_bot_positions.at(it).at(0)*20 + 10 - 20, initial_bot_positions.at(it).at(1)*20 + 10 - 20),
        Point(initial_bot_positions.at(it).at(0)*20 + 10 + 20, initial_bot_positions.at(it).at(1)*20 + 10 + 20), Scalar(255), CV_FILLED, 8, 0);
/*  namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
  imshow( "Display window", bin_corridor_img_large );                   // Show our image inside it.

  waitKey(0);*/

  std::vector<std::vector<Point>> contours_coordinates;
  Mat hierarchy;

  findContours(bin_corridor_img_large, contours_coordinates, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
  Scalar color = Scalar(255, 255, 255);

  corridor_contours_img = Mat::zeros(bin_corridor_img_large.size(), CV_8UC1);
  for (int i = 0; i < contours_coordinates.size(); i++) {

    drawContours(corridor_contours_img, contours_coordinates, i, color, 1  , LINE_4, hierarchy, 0);
  }

if (!EFFICIENT_ENVIRONMENT)
  dilate(corridor_contours_img, corridor_contours_img, Mat(), Point(-1, -1), 2, 1, 1);


    //

//Mat element = getStructuringElement(MORPH_RECT, Size(2, 2), Point(1,1) );

}

void EnvironmentGenerator::makeRooms() {

  Mat bin_corridor_img_large_inv = Mat::zeros(environment_width * 20, environment_height * 20, CV_8UC1);
  bitwise_not(bin_corridor_img_large,bin_corridor_img_large_inv);
  corridor_contours_img.copyTo(corridor_contours_img_save);
  std::vector<std::vector<Point>> contours_coordinates;
  Mat hierarchy;

  findContours(bin_corridor_img_large_inv, contours_coordinates, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

  for(int i = 0; i <contours_coordinates.size();i++ )
  {
    Rect boundRect=boundingRect(contours_coordinates.at(i));
       for(int itx = boundRect.x; itx<(boundRect.x + boundRect.width); itx++)
    {
      for(int ity = boundRect.y; ity<(boundRect.y + boundRect.height); ity++)
      {

        std::vector<int> coord_mod_rooms{(itx-boundRect.x) % (int)(boundRect.width /2), (ity-boundRect.y) % (int)(boundRect.height /2)};
        if( boundRect.width<(float)environment_width * 20*ROOM_PERCENTAGE)
          coord_mod_rooms.at(0) = 1;
        if(boundRect.height<(float)environment_height * 20*ROOM_PERCENTAGE)
          coord_mod_rooms.at(1) = 1;

    if ((coord_mod_rooms.at(0) == 0 || coord_mod_rooms.at(1) == 0))
        if (bin_corridor_img_large.at<uchar>(ity, itx) == 0) {
   if (EFFICIENT_ENVIRONMENT)
             rectangle(corridor_contours_img, Point(itx, ity ), Point(itx , ity ), Scalar(255), 1, 8, 0);
   else
             rectangle(corridor_contours_img, Point(itx-1, ity-1 ), Point(itx+1 , ity+1 ), Scalar(255), 1, 8, 0);

        }
      }
    }
  }


/*
  for (int itx = 0; itx < environment_width * 20; itx++) {
    for (int ity = 0; ity < environment_height * 20; ity++) {

      vector<int> coord_mod_rooms{itx % (int)(environment_width * 20 * room_percentage), ity % (int)(environment_height * 20 * room_percentage)};

      if ((coord_mod_rooms.at(0) == 0 || coord_mod_rooms.at(1) == 0))
        if (bin_corridor_img_large.at<uchar>(ity, itx) == 0) {
          rectangle(corridor_contours_img, Point(itx - 1, ity - 1), Point(itx + 1, ity + 1), Scalar(255), 1, 8, 0);
        }
    }
  }*/
}

void EnvironmentGenerator::makeRandomOpenings() {

  //RNG rng(cv::getTickCount());
  int half_size_openings = 13;
  int erosion_size = 1;
  Mat element = getStructuringElement(cv::MORPH_CROSS,
         cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
         cv::Point(erosion_size, erosion_size) );
  Mat corridor_contours_img_temp = Mat::zeros(corridor_contours_img.size(), CV_8UC1);


if (EFFICIENT_ENVIRONMENT)
  corridor_contours_img_save.copyTo(corridor_contours_img_temp);
else
  erode(corridor_contours_img_save, corridor_contours_img_temp, element, Point(-1, -1), 2, 1, 1);

  for (int itx = 0; itx < environment_width * 20; itx++) {
    for (int ity = 0; ity < environment_height * 20; ity++) {
        if(corridor_contours_img_temp.at<uchar>(ity, itx)==255&&  rng.uniform(1, 1000)<AMOUNT_OF_OPENINGS
            &&itx!=0&&itx!=environment_width* 20
            &&itx!=0&&itx!=environment_height* 20)
        {
          corridor_contours_img.at<uchar>(ity, itx)==0;
          rectangle(corridor_contours_img, Point(itx - half_size_openings, ity - half_size_openings), Point(itx + half_size_openings, ity + half_size_openings), Scalar(0), CV_FILLED, 8, 0);

        }
    }
  }

/*
  namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
  imshow( "Display window", corridor_contours_img );                   // Show our image inside it.

  waitKey(0);*/
/*  for (int it = 0; it < amount_of_openings; it++) {
    vector<int> random_coordinate{rng.uniform(half_size_openings, environment_height * 20 - half_size_openings), rng.uniform(half_size_openings, environment_width * 20 - half_size_openings)};
    rectangle(corridor_contours_img, Point(random_coordinate.at(0) - half_size_openings, random_coordinate.at(1) - half_size_openings), Point(random_coordinate.at(0) + half_size_openings, random_coordinate.at(1) + half_size_openings), Scalar(0), CV_FILLED, 8, 0);
    //circle(corridor_contours_img, Point(random_coordinate.at(0), random_coordinate.at(1)), half_size_openings, Scalar(0), -1, 8, 0);


  }*/

}

const int dx[] = {+1, 0, -1, 0};
const int dy[] = {0, +1, 0, -1};
void EnvironmentGenerator::dfs(int x, int y, int current_label) {
  if (x < 0 || x == environment_width) return; // out of bounds
  if (y < 0 || y == environment_height) return; // out of bounds
  if (connectivity_labels.at(x).at(y) || !environment_grid.at(x).at(y).is_corridor_present) return; // already labeled or not marked with 1 in m

  // mark the current cell
  connectivity_labels.at(x).at(y) = current_label;

  // recursively mark the neighbors
  for (int direction = 0; direction < 4; ++direction)
    dfs(x + dx[direction], y + dy[direction], current_label);
}

double get_value_at_line(std::string file_name, unsigned int num) {

   std::fstream file(file_name);

   file.seekg(std::ios::beg);
   for(int i=0; i < num - 1; ++i){
     file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
   }

   double value;
   file >> value;

   return value;

}
