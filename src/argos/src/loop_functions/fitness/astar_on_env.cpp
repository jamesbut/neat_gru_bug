#include "astar_on_env.h"
#include "search_grid_wrapper.h"
#include <aiTools/Math/Vector2.h>
#include <aiTools/Algorithm/AStar.h>

using namespace argos;
using namespace aiTools;

std::vector<CVector2> astar_on_env(const std::string env_path) {

   Math::Vector2<int> start_pos = aiTools::Math::Vector2<int>(20,20);
   Math::Vector2<int> goal_pos = aiTools::Math::Vector2<int>(110,110);
   SearchGridWrapper searchGrid(env_path, start_pos, goal_pos);
   Algorithm::AStarAlgorithm<Algorithm::SampleAStarSearchGraphWrapper<Grid<SearchNode>::GridIndex, SearchNode::cost_type>> algorithm(searchGrid);

   //auto coords = SearchGridWrapper::value_type(start_pos.x, goal_pos.y);
	//searchGrid.updateCostSetOpen(coords, 0);

   unsigned int iterations = 0;
   while(!algorithm.isComplete()) {
			++iterations;
			if(iterations % 1000 == 0)
				std::cout << iterations << std::endl;
			algorithm.performIteration();
	}

   auto path = algorithm.getPath();

   for(auto iterator = path.first; iterator != path.second; iterator++) {
      size_t x, y;
      std::tie(x, y) = *iterator;
      std::cout << x << " " << y << std::endl;

   }

   SearchGridWrapper::printGridToPng("", searchGrid.mData);

   //Convert environment into appropriate form for astar library

   //Retrieve astar path

   //Convert returned astar into vector of CVector2

   //Default return for now
   CVector2 vec = CVector2(0.0, 0.0);
   std::vector<CVector2> vec_of_vecs;
   vec_of_vecs.push_back(vec);
   return vec_of_vecs;

}
