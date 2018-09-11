#ifndef _SEARCH_GRID_WRAPPER_H_
#define _SEARCH_GRID_WRAPPER_H_

#include <aiTools/Algorithm/AStar.h>
#include <aiTools/Util/Grid.h>
#include <aiTools/Math/Vector2.h>
#include <queue>
#include <set>
#include <memory>

//OpenCV libraries
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

#include "search_node_comparator.h"

struct SearchNode
{
	typedef double cost_type;

	cost_type cost{-1};
	cost_type traversalCost{1};
	bool isOpen{false};
	bool isClosed{false};
	bool isBlocked{false};
	bool isGoal{false};
	bool isPath{false};
};

typedef std::tuple<size_t, size_t> IndexType;

typedef boost::multi_index_container
	<
		IndexType,
		boost::multi_index::indexed_by
		<
			boost::multi_index::hashed_unique<boost::multi_index::identity<IndexType>>,
			boost::multi_index::ordered_non_unique<boost::multi_index::identity<IndexType>, SearchNodeComparator>
		>
	> OpenListType;


struct SearchGridWrapper : public aiTools::Algorithm::SampleAStarSearchGraphWrapper<aiTools::Grid<SearchNode>::GridIndex, SearchNode::cost_type>
{
	typedef aiTools::Grid<SearchNode> GridType;
	typedef GridType::GridIndex GridIndex;

	//methods that star needs:
	void updateCostSetOpen(value_type& node, cost_type cost);
	void updateCostSetOpen(value_type& node, cost_type newCosts, value_type& parentNode);

	cost_type getCostForNode(const value_type& node);
	void setExpanded(value_type& node);
	bool isExpanded(value_type& node);
	bool isGoalNode(value_type& node);
	boost::optional<value_type> removeBestOpen();
	cost_type getCostBetween(value_type& parent, value_type& child);

	void forAllSuccessorNodes(value_type& node, std::function<void(value_type&)> action);
	void forAllPredecessorNodes(value_type& node, BacktrackAction& action);

	//methods that astar (additionally) needs:

	bool reOpenIf(value_type& node, std::function<bool(const value_type&)> action);
	cost_type getHeuristic(const value_type& node);

	static aiTools::Grid<SearchNode> gridFromPng(const std::string& filename);
	static aiTools::Grid<SearchNode> gridFromMat(const cv::Mat& mat_img);
	static void printGridToPng(const std::string& filename, aiTools::Grid<SearchNode>& grid);

	//now, own stuff:
	//SearchGridWrapper(const std::string& initialLayout);
	SearchGridWrapper(const std::string& filePath,
							const aiTools::Math::Vector2<int> start_pos,
							const aiTools::Math::Vector2<int> goal_pos);
	SearchGridWrapper(const cv::Mat& mat_img,
                     const aiTools::Math::Vector2<int> start_pos,
                     const aiTools::Math::Vector2<int> goal_pos);

	void initialise(const aiTools::Math::Vector2<int> start_pos,
						 const aiTools::Math::Vector2<int> goal_pos);

	aiTools::Grid<SearchNode> mData;
	GridIndex mGoalIndex;

	//Priority queue for open nodes
	//std::unique_ptr <std::priority_queue<value_type, std::vector<value_type>, SearchNodeComparator>> mOpenQueue;
	std::unique_ptr <OpenListType> mOpenQueue;

};

#endif
