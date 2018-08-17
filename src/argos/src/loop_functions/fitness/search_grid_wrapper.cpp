#include "search_grid_wrapper.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <aiTools/Math/Distance.h>

using namespace cv;

SearchGridWrapper::SearchGridWrapper(const std::string& filePath,
                                     const aiTools::Math::Vector2<int> start_pos,
                                     const aiTools::Math::Vector2<int> goal_pos) :
                                     mData(gridFromPng(filePath)) {

      //Set start position
      mData.at(start_pos.x, start_pos.y).mData.cost = 0;
      mData.at(start_pos.x, start_pos.y).mData.isOpen = true;
      mData.at(start_pos.x, start_pos.y).mData.isClosed = false;

      //Set goal
      mGoalIndex = GridIndex(goal_pos.x, goal_pos.y);
      mData.at(goal_pos.x, goal_pos.y).mData.isGoal = true;

}

void SearchGridWrapper::updateCostSetOpen(SearchGridWrapper::value_type& node, SearchGridWrapper::cost_type cost) {

   auto contentNode = mData.at(node);
	auto& current = contentNode.mData;

	if((!current.isOpen && !current.isClosed) || cost < current.cost) //if unknown OR costs are lowered...
	{
		//else, do not update/re-open! (if new cost are actually higher.)
		current.cost = cost;
		current.isOpen = true;
		current.isClosed = false;
	}

}

void SearchGridWrapper::updateCostSetOpen(SearchGridWrapper::value_type& node,
                                          SearchGridWrapper::cost_type newCosts,
                                          SearchGridWrapper::value_type& parentNode) {

   updateCostSetOpen(node, newCosts);
	//TODO...
	//mData.at(node).mData.reachedFrom =

}

SearchGridWrapper::cost_type SearchGridWrapper::getCostForNode(const value_type& node) {

   return mData.at(node).mData.cost;

}

void SearchGridWrapper::setExpanded(value_type& node) {

   mData.at(node).mData.isOpen = false;
	mData.at(node).mData.isClosed = true;

}

bool SearchGridWrapper::isExpanded(value_type& node) {

   return mData.at(node).mData.isClosed;

}

bool SearchGridWrapper::isGoalNode(value_type& node) {

   return mData.at(node).mData.isGoal;

}

boost::optional<SearchGridWrapper::value_type> SearchGridWrapper::removeBestOpen() {

   boost::optional<SearchGridWrapper::value_type> result;
   SearchGridWrapper::cost_type bestCosts{};

   	for(std::size_t y = 0;y<mData.mHeight;++y)
   	{
   		for(std::size_t x = 0;x<mData.mWidth;++x)
   		{
   			value_type current{x, y};
   			auto& node = mData.at(current).mData;

   			if(!node.isOpen) //only ever look at open nodes
   				continue;

   			if(!result)
   			{
   				result = current;
   				bestCosts = node.cost;
   				//std::cout << "first open node is " << x << "," << y << ", cost: " << bestCosts << ", h: " << getHeuristic(current) << std::endl;
   			}
   			else
   			{
   				auto totalEstCostFirst = node.cost + getHeuristic(current);
   				auto totalEstCostSecond = bestCosts + getHeuristic(*result);

   				//std::cout << "node is " << x << "," << y << ", cost: " << node.cost << ", h: " << getHeuristic(current) << ", both: " << totalEstCostFirst;

   				if(totalEstCostFirst == totalEstCostSecond)
   				{
   					//std::cout << ", costs equal";

   					if(node.cost > bestCosts) //counter-intuitive tie-break with greater cost - means greater distance travelled, more progress (hopefully)
   					{
   						//std::cout << ", tie break winner";
   						result = current;
   						bestCosts = node.cost;
   					}
   				}
   				else if(totalEstCostFirst < totalEstCostSecond)
   				{
   					//std::cout << ", costs lower";
   					result = current;
   					bestCosts = node.cost;
   				}

   				//std::cout << std::endl;
   			}
   		}
   	}
   	//std::cout << std::endl<< std::endl<< std::endl;

   	//*remove* best open - mark as not open anymore
   	if(result)
   		mData.at(*result).mData.isOpen = false;

   	return result;

}

SearchGridWrapper::cost_type SearchGridWrapper::getCostBetween(value_type& parent, value_type& child) {

   return 1;

}

void SearchGridWrapper::forAllSuccessorNodes(SearchGridWrapper::value_type& parent, std::function<void(SearchGridWrapper::value_type&)> action) {

   mData.forAll4NeighborsDo(parent,
								[this, action](GridType::node_type neighbor)
								{
									auto node = neighbor.mData;
									if(!node.isBlocked)
									{
										SearchGridWrapper::value_type neighborIndex(neighbor.x, neighbor.y);
										action(neighborIndex);
									}
								}
							);

}

void SearchGridWrapper::forAllPredecessorNodes(SearchGridWrapper::value_type& parent, BacktrackAction& action) {

   mData.forAll4NeighborsDo(parent,
								[this, &action](GridType::node_type neighbor)
								{
									auto node = neighbor.mData;
									if(node.isClosed)
									{
										SearchGridWrapper::value_type neighborIndex(neighbor.x, neighbor.y);
										action(neighborIndex);
									}
								}
							);

}

// //methods that astar (additionally) needs:

bool SearchGridWrapper::reOpenIf(value_type& node, std::function<bool(const value_type&)> action) {

   //find node in closed or open list
	auto& foundNode = mData.at(node).mData;
	if(foundNode.isClosed || foundNode.isOpen) //"found" node, is it closed?
	{
		bool needsReopening = action(node);
		return needsReopening;
	}
	return true; //no such closed node, needs opening

}

SearchGridWrapper::cost_type SearchGridWrapper::getHeuristic(const value_type& node) {

   typedef aiTools::Math::Vector2<std::size_t> Vec;

	Vec goalPos{mGoalIndex};
	Vec nowPos{node};

	double distance = aiTools::Math::manhattan_distance(goalPos.begin(), goalPos.end(), nowPos.begin(), nowPos.end());
	//manhattan_distance
	//euclidean_distance
	//chebyshev_distance

	 return cost_type(distance);

}

aiTools::Grid<SearchNode> SearchGridWrapper::gridFromPng(const std::string& filename) {

   //Read in environment
   Mat read_img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

   if(!read_img.data) std::cout << "Could not read image for astar" << std::endl;

   aiTools::Grid<SearchNode> map(read_img.cols, read_img.rows);

   for(size_t y = 0; y < map.mHeight; y++) {

      for(size_t x = 0; x < map.mWidth; x++) {

         int img_value = read_img.at<int>(x,y);
         if(img_value == 255)
            map.at(x, y).mData.isBlocked = true;
         else
            map.at(x, y).mData.traversalCost = 1;

         //Add start state

      }

   }

   // for(int i = 0; i < read_img.rows; i++) {
   //    const int* img_col_ptr = read_img.ptr<int>(i);
   //    for(int j = 0; j < read_img.cols; j++){
   //       std::cout << img_col_ptr[j] << " ";
   //    }
   //    std::cout << std::endl;
   // }

   //std::vector<std::vector<bool>> occupancy_grid(read_img.rows, std::vector<bool>(read_img.cols, false));

   // for(int i = 0; i < occupancy_grid.size(); i++) {
   //    for(int j = 0; j < occupancy_grid[i].size(); j++) {
   //       if(read_img.at<int>(i,j) == 255) occupancy_grid[i][j] = true;
   //       std::cout << occupancy_grid[i][j];
   //    }
   //    std::cout << std::endl;
   // }

   // namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
   // imshow( "Display window", read_img );                   // Show our image inside it.
   //
   // waitKey(0);


}

// static void SearchGridWrapper::printGridToPng(const std::string& filename, aiTools::Grid<SearchNode>& grid) {}

namespace aiTools {

   const std::vector<Direction::Type>  Direction::AllDirections = {Left,Up,Right,Down,LeftUp,RightUp,LeftDown,RightDown};
   const std::vector<Direction::Type>  Direction::VonNeumannDirections = {Left,Up,Right,Down};
   const std::vector<Direction::Type>  Direction::MooreDirections = {Left,Up,Right,Down,LeftUp,RightUp,LeftDown,RightDown};

}
