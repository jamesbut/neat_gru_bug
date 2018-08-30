#include "search_grid_wrapper.h"

#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <aiTools/Math/Distance.h>
#include <aiTools/Math/LinearTransformation.h>
#include <algorithm>

#include <boost/filesystem.hpp>

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

      //Initialise open priority queue
      //mOpenQueue.reset(new std::priority_queue<value_type, std::vector<value_type>, SearchNodeComparator>(SearchNodeComparator(*this)));
      mOpenQueue.reset(new std::set<value_type, SearchNodeComparator>(SearchNodeComparator(*this)));

      //Add start pos to open queue
      //mOpenQueue->push(std::make_tuple(start_pos.x, start_pos.y));
      mOpenQueue->insert(std::make_tuple(start_pos.x, start_pos.y));

}

void SearchGridWrapper::updateCostSetOpen(SearchGridWrapper::value_type& node, SearchGridWrapper::cost_type cost) {

   auto contentNode = mData.at(node);
	auto& current = contentNode.mData;

	if((!current.isOpen && !current.isClosed) || cost < current.cost) //if unknown OR costs are lowered...
	{
		//else, do not update/re-open! (if new cost are actually higher.)

      if(current.isOpen) {
         //std::unique_ptr<std::priority_queue<value_type, std::vector<value_type>, SearchNodeComparator>> copy_queue(new std::priority_queue<value_type, std::vector<value_type>, SearchNodeComparator>(SearchNodeComparator(*this)));

         auto iter = mOpenQueue->find(node);
         if(iter != mOpenQueue->end()) {
            mOpenQueue->erase(iter);
         }
         //
         // while(!mOpenQueue->empty()) {
         //    if(mOpenQueue->find() != node)
         //       copy_queue->push(mOpenQueue->top());
         //    mOpenQueue->pop();
         // }

         // mOpenQueue.swap(copy_queue);

      }

		current.cost = cost;
		current.isOpen = true;
		current.isClosed = false;

      //Here is where I want to add the new open node to the priority queue
      //mOpenQueue->push(node);
      mOpenQueue->insert(node);

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

   //Here I just want to pop next off priority queue and use that
   //rather than search through grid all the time
   if(mOpenQueue->empty())
      return boost::optional<SearchGridWrapper::value_type>();

   auto result = *(mOpenQueue->begin());
   mOpenQueue->erase(mOpenQueue->begin());
   //mOpenQueue->pop();

   mData.at(result).mData.isOpen = false;

   return result;

   // boost::optional<SearchGridWrapper::value_type> result;
   // SearchGridWrapper::cost_type bestCosts{};
   //
   // 	for(std::size_t y = 0;y<mData.mHeight;++y)
   // 	{
   // 		for(std::size_t x = 0;x<mData.mWidth;++x)
   // 		{
   // 			value_type current{x, y};
   // 			auto& node = mData.at(current).mData;
   //
   // 			if(!node.isOpen)
   //             continue;
   //
   // 			if(!result)
   // 			{
   // 				result = current;
   // 				bestCosts = node.cost;
   // 				//std::cout << "first open node is " << x << "," << y << ", cost: " << bestCosts << ", h: " << getHeuristic(current) << std::endl;
   // 			}
   // 			else
   // 			{
   //
   // 				auto totalEstCostFirst = node.cost + getHeuristic(current);
   // 				auto totalEstCostSecond = bestCosts + getHeuristic(*result);
   //
   // 				//std::cout << "node is " << x << "," << y << ", cost: " << node.cost << ", h: " << getHeuristic(current) << ", both: " << totalEstCostFirst;
   //
   // 				if(totalEstCostFirst == totalEstCostSecond)
   // 				{
   // 					//std::cout << ", costs equal";
   //
   // 					if(node.cost > bestCosts) //counter-intuitive tie-break with greater cost - means greater distance travelled, more progress (hopefully)
   // 					{
   // 						//std::cout << ", tie break winner";
   // 						result = current;
   // 						bestCosts = node.cost;
   // 					}
   // 				}
   // 				else if(totalEstCostFirst < totalEstCostSecond)
   // 				{
   // 					//std::cout << ", costs lower";
   // 					result = current;
   // 					bestCosts = node.cost;
   // 				}
   //
   // 				//std::cout << std::endl;
   // 			}
   // 		}
   // 	}
   //
   // 	//*remove* best open - mark as not open anymore
   // 	if(result) {
   //       //std::cout << mData.at(*result).x << " " << mData.at(*result).y << std::endl;
   //       mData.at(*result).mData.isOpen = false;
   //    }
   //
   //
   // 	return result;

}

SearchGridWrapper::cost_type SearchGridWrapper::getCostBetween(value_type& parent, value_type& child) {

   typedef aiTools::Math::Vector2<std::size_t> Vec;
	Vec from{parent};
	Vec to{child};

	double distance = (to-from).length<double>();

	cost_type parentCost = mData.at(parent).mData.traversalCost;
	cost_type childCost = mData.at(child).mData.traversalCost;

	double avg = (parentCost+childCost) / 2.;
	double cost = avg * distance;

	return cost_type(cost);

   //return 1;

}

void SearchGridWrapper::forAllSuccessorNodes(SearchGridWrapper::value_type& parent, std::function<void(SearchGridWrapper::value_type&)> action) {

   mData.forAll8NeighborsDo(parent,
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

   mData.forAll8NeighborsDo(parent,
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
   //double distance = aiTools::Math::euclidean_distance(goalPos.begin(), goalPos.end(), nowPos.begin(), nowPos.end());
	//manhattan_distance
	//euclidean_distance
	//chebyshev_distance
   //return 0;
	return cost_type(distance);

}

const int MAX_NUM_READ_TIMES = 10;

aiTools::Grid<SearchNode> SearchGridWrapper::gridFromPng(const std::string& filename) {

   if(!boost::filesystem::exists(filename))
      std::cout << "File: " << filename << " does not exist" << std::endl;

   //Read in environment
   // Mat read_img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
   //
   // if(!read_img.data) {
   //    std::cout << "Could not read image for astar" << std::endl;
   //    std::cout << "Filename: " << filename << std::endl;
   // }

   Mat read_img;
   int num_reads = 0;

   //Keep trying to read file if can't at first
   //This occasionally happens, I think it is something to do with the
   //synchronous processes
   do {

      read_img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
      num_reads++;

      if(num_reads > MAX_NUM_READ_TIMES) {

         std::cout << "Could not read image for astar" << std::endl;
         std::cout << "Exceeded number of file reads, file reads: " << num_reads << std::endl;
         break;

      }

   } while (!read_img.data);


   aiTools::Grid<SearchNode> map(read_img.rows, read_img.cols);

   for(size_t y = 0; y < map.mHeight; y++) {

      for(size_t x = 0; x < map.mWidth; x++) {

         int img_value = static_cast<int>(read_img.at<uchar>(x,y));

         if(img_value == 255)
            map.at(x, y).mData.isBlocked = true;
         else
            map.at(x, y).mData.traversalCost = 1;

      }

   }

   // namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
   // imshow( "Display window", read_img );                   // Show our image inside it.
   //
   // waitKey(0);

   return map;

}

SearchNode::cost_type findHighestCost(aiTools::Grid<SearchNode>& grid) {

	SearchNode::cost_type maxCost = 0;

	std::for_each(grid.mData.begin(), grid.mData.end(),
			[&maxCost](SearchNode& sn)
			{
				if(!sn.isBlocked)
					maxCost = std::max(maxCost, sn.cost);
			});

	return maxCost;

}

void SearchGridWrapper::printGridToPng(const std::string& filename, aiTools::Grid<SearchNode>& grid) {
   //std::cout << "PNG print out :" << std::endl;
   Mat img(grid.mHeight, grid.mWidth, CV_8UC3, Scalar(0, 0, 0));

   for(size_t y = 0; y < grid.mHeight; y++) {

      for(size_t x = 0; x < grid.mWidth; x++) {

         auto& cell = grid.at(x,y).mData;

         if(cell.isBlocked)
            img.at<Vec3b>(x,y) = Vec3b(255, 255, 255);
         if(cell.isClosed) {
            //std::cout << x << " " << y << std::endl;
            img.at<Vec3b>(x,y) = Vec3b(255, 0, 0);
         }
         if(cell.isGoal)
            img.at<Vec3b>(x,y) = Vec3b(0, 255, 0);

      }

   }

   namedWindow("Astar search", WINDOW_AUTOSIZE);// Create a window for display.
   imshow("Astar search", img);                   // Show our image inside it.

   waitKey(0);

}

namespace aiTools {

   const std::vector<Direction::Type>  Direction::AllDirections = {Left,Up,Right,Down,LeftUp,RightUp,LeftDown,RightDown};
   const std::vector<Direction::Type>  Direction::VonNeumannDirections = {Left,Up,Right,Down};
   const std::vector<Direction::Type>  Direction::MooreDirections = {Left,Up,Right,Down,LeftUp,RightUp,LeftDown,RightDown};

}
