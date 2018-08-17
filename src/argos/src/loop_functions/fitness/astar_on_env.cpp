#include "astar_on_env.h"
#include "search_grid_wrapper.h"
#include <aiTools/Math/Vector2.h>

using namespace argos;

std::vector<CVector2> astar_on_env(const std::string env_path) {

   aiTools::Math::Vector2<int> start_pos = aiTools::Math::Vector2<int>(2,2);
   aiTools::Math::Vector2<int> goal_pos = aiTools::Math::Vector2<int>(10,10);
   SearchGridWrapper searchGrid(env_path, start_pos, goal_pos);

   //Convert environment into appropriate form for astar library

   //Retrieve astar path

   //Convert returned astar into vector of CVector2

   //Default return for now
   CVector2 vec = CVector2(0.0, 0.0);
   std::vector<CVector2> vec_of_vecs;
   vec_of_vecs.push_back(vec);
   return vec_of_vecs;

}
