#include "astar_on_env.h"
#include "search_grid_wrapper.h"
#include <aiTools/Math/Vector2.h>
#include <aiTools/Algorithm/AStar.h>

using namespace argos;
using namespace aiTools;

std::vector<CVector2> astar_on_env(const std::string& env_path) {

   Math::Vector2<int> start_pos = aiTools::Math::Vector2<int>(20,20);
   Math::Vector2<int> goal_pos = aiTools::Math::Vector2<int>(110,110);
   SearchGridWrapper searchGrid(env_path, start_pos, goal_pos);
   Algorithm::AStarAlgorithm<Algorithm::SampleAStarSearchGraphWrapper<Grid<SearchNode>::GridIndex, SearchNode::cost_type>> algorithm(searchGrid);
   //std::cout << "Starting search" << std::endl;
   unsigned int iterations = 0;
   while(!algorithm.isComplete()) {
			++iterations;
			//if(iterations % 1000 == 0)
            //std::cout << iterations << std::endl;

			algorithm.performIteration();
	}

   //std::cout << "Iterations: " << iterations << std::endl;

   //Retrieve astar path
   auto path = algorithm.getPath();

   std::vector<CVector2> path_vec;

   //Print path
   for(auto iterator = path.first; iterator != path.second; iterator++) {
      size_t x, y;
      std::tie(x, y) = *iterator;

      //Convert returned astar into vector of CVector2
      path_vec.push_back(CVector2(x, y));

      //std::cout << x << " " << y << std::endl;
   }

   //SearchGridWrapper::printGridToPng("Astar search", searchGrid.mData);

   return path_vec;

}

//This is to work with the environment generator
std::vector<CVector2> astar_on_env(const cv::Mat& mat_img, const argos::CVector2 start_position,
                                                           const argos::CVector2 goal_position) {

   Math::Vector2<int> start_pos = aiTools::Math::Vector2<int>(start_position.GetX(), start_position.GetY());
   Math::Vector2<int> goal_pos = aiTools::Math::Vector2<int>(goal_position.GetX(), goal_position.GetY());
   SearchGridWrapper searchGrid(mat_img, start_pos, goal_pos);
   Algorithm::AStarAlgorithm<Algorithm::SampleAStarSearchGraphWrapper<Grid<SearchNode>::GridIndex, SearchNode::cost_type>> algorithm(searchGrid);
   // //std::cout << "Starting search" << std::endl;
   unsigned int iterations = 0;
   while(!algorithm.isComplete()) {
			++iterations;
			//if(iterations % 1000 == 0)
            //std::cout << iterations << std::endl;

			algorithm.performIteration();
	}

   //std::cout << "Iterations: " << iterations << std::endl;

   //Retrieve astar path
   auto path = algorithm.getPath();

   std::vector<CVector2> path_vec;

   //Print path
   for(auto iterator = path.first; iterator != path.second; iterator++) {
      size_t x, y;
      std::tie(x, y) = *iterator;

      //Convert returned astar into vector of CVector2
      path_vec.push_back(CVector2(x, y));

      //std::cout << x << " " << y << std::endl;
   }

   //SearchGridWrapper::printGridToPng("Astar search", searchGrid.mData);

   return path_vec;

}
