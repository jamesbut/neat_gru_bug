#include "search_node_comparator.h"
#include "search_grid_wrapper.h"

bool SearchNodeComparator::operator()(const index_type& n1, const index_type& n2)
{

   auto totalCostFirst = mSearchGraphWrapper.getCostForNode(n1) + mSearchGraphWrapper.getHeuristic(n1);
   auto totalCostSecond = mSearchGraphWrapper.getCostForNode(n2) + mSearchGraphWrapper.getHeuristic(n2);

   if(totalCostFirst < totalCostSecond) return true;

   if(totalCostFirst == totalCostSecond) {

      if(mSearchGraphWrapper.getCostForNode(n1) > mSearchGraphWrapper.getCostForNode(n2))
         return true;

   }

   return false;

}
