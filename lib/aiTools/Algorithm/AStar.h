// aiTools is a free C++ library of AI tools.
// Copyright (C) 2013 - 2016  Benjamin Schnieders

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program, see file LICENCE.txt.
// Alternativley, see <http://www.gnu.org/licenses/>.

#ifndef __AITOOLS_ASTAR_H_INCLUDED
#define __AITOOLS_ASTAR_H_INCLUDED

#include <boost/optional.hpp>

#include <list>

#include<iostream>

namespace aiTools
{
	namespace Algorithm
	{
		namespace astar_detail
		{
			template <typename SearchGraphWrapper>
			struct NodeComparator
			{
				typedef typename SearchGraphWrapper::value_type value_type;
				typedef typename SearchGraphWrapper::cost_type cost_type;

				NodeComparator(SearchGraphWrapper& searchGraphWrapper, value_type& goalNode);
				void operator()(const value_type& node);

				value_type mLowestCostNode;
			private:
				SearchGraphWrapper& mSearchGraphWrapper;
			};

			template <typename SearchGraphWrapper>
			NodeComparator<SearchGraphWrapper>::NodeComparator(SearchGraphWrapper& searchGraphWrapper, value_type& goalNode)
				:mLowestCostNode(goalNode), mSearchGraphWrapper(searchGraphWrapper)
			{
			}

			template <typename SearchGraphWrapper>
			void NodeComparator<SearchGraphWrapper>::operator()(const value_type& node)
			{
				cost_type storedCost = mSearchGraphWrapper.getCostForNode(mLowestCostNode);
				cost_type newCost = mSearchGraphWrapper.getCostForNode(node);
				//std::cout << storedCost << " " << newCost << std::endl;
				if(newCost < storedCost)
					mLowestCostNode = node;
				if(newCost == storedCost) //tie-breaking over heuristic, if possible:
				{
					cost_type storedHeuristic = mSearchGraphWrapper.getHeuristic(mLowestCostNode);
					cost_type newHeuristic = mSearchGraphWrapper.getHeuristic(node);
					if(newHeuristic < storedHeuristic)
						mLowestCostNode = node;
				}
			}

		}

		template <typename SearchGraphWrapper>
		struct AStarAlgorithm
		{
			typedef typename SearchGraphWrapper::value_type value_type;
			typedef typename SearchGraphWrapper::cost_type cost_type;
			typedef SearchGraphWrapper SearchGraphWrapperType;
			typedef std::list<value_type> PathType;

			AStarAlgorithm(SearchGraphWrapper& wrapper, value_type* startNode = NULL);

			void performIteration();
			bool isComplete() const;
			bool wasSuccessful() const;

			void backtrackFrom(value_type& node);

			std::pair<typename PathType::const_iterator, typename PathType::const_iterator> getPath() const;
		private:
			SearchGraphWrapper& mSearchGraphWrapper;
			bool mIsFinished;
			bool mWasSuccessful;
			PathType mPath;
		};

		template <typename SearchGraphWrapper>
		AStarAlgorithm<SearchGraphWrapper>::AStarAlgorithm(SearchGraphWrapper& wrapper, typename SearchGraphWrapper::value_type* startNode)
			: mSearchGraphWrapper(wrapper), mIsFinished(false), mWasSuccessful(false)
		{
			if(startNode)
				mSearchGraphWrapper.updateCostSetOpen(*startNode, cost_type(0));
		}

		template <typename SearchGraphWrapper>
		bool AStarAlgorithm<SearchGraphWrapper>::isComplete() const
		{
			return mIsFinished;
		}

		template <typename SearchGraphWrapper>
		bool AStarAlgorithm<SearchGraphWrapper>::wasSuccessful() const
		{
			return mWasSuccessful;
		}

		template <typename SearchGraphWrapper>
		void AStarAlgorithm<SearchGraphWrapper>::performIteration()
		{
			boost::optional<value_type> optional_node = mSearchGraphWrapper.removeBestOpen();

			if(!optional_node)
			{
				mPath.clear();
				mIsFinished = true;
				mWasSuccessful = false;
				return;
			}

			value_type& parentNode = *optional_node; //TODO 123 check
			mSearchGraphWrapper.setExpanded(parentNode);

			cost_type parentCost = mSearchGraphWrapper.getCostForNode(parentNode);

			mSearchGraphWrapper.forAllSuccessorNodes(parentNode,
					[this, &parentNode, &parentCost](value_type& successor)
					{
						if(isComplete())
							return;

						const cost_type costForEdge = mSearchGraphWrapper.getCostBetween(parentNode, successor);
						const cost_type newCostForNode = parentCost + costForEdge;

						//re-opening is sometimes needed, if heuristic is admissible, but not consistent (monotonic)
						//could re-use heuristic value from closed list node here.
						//re-open a node from the closed or open list
						bool reopenNode = mSearchGraphWrapper.reOpenIf(successor,
								[this, &newCostForNode](const value_type& closedNode)
								{
									const cost_type costStoredInList = mSearchGraphWrapper.getCostForNode(closedNode);
									return newCostForNode < costStoredInList;
								}
							);
						if(!reopenNode)
							return;

						mSearchGraphWrapper.updateCostSetOpen(successor, newCostForNode, parentNode);

						if(mSearchGraphWrapper.isGoalNode(successor)){
							//std::cout << "Found goal, backtracking" << std::endl;
							backtrackFrom(successor);
						}

					}
				);
		}

		template <typename SearchGraphWrapper>
		void AStarAlgorithm<SearchGraphWrapper>::backtrackFrom(value_type& node)
		{
			mPath.clear();

			astar_detail::NodeComparator<SearchGraphWrapper> lowestCostNodeGetter(mSearchGraphWrapper, node);

			while(mSearchGraphWrapper.getCostForNode(lowestCostNodeGetter.mLowestCostNode) != cost_type(0))
			{
				mPath.push_front(lowestCostNodeGetter.mLowestCostNode);

				mSearchGraphWrapper.forAllPredecessorNodes(mPath.front(), lowestCostNodeGetter);
			}
			mPath.push_front(lowestCostNodeGetter.mLowestCostNode);

			mIsFinished = true;
			mWasSuccessful = true;
		}

		template <typename SearchGraphWrapper>
		std::pair<typename AStarAlgorithm<SearchGraphWrapper>::PathType::const_iterator, typename AStarAlgorithm<SearchGraphWrapper>::PathType::const_iterator> AStarAlgorithm<SearchGraphWrapper>::getPath() const
		{
			return std::make_pair(mPath.begin(), mPath.end());
		}

		template <typename T, typename C>
      struct SampleAStarSearchGraphWrapper
      {
      	//typedefs that astar needs
      	typedef T value_type;
      	typedef C cost_type;

      	typedef aiTools::Algorithm::astar_detail::NodeComparator<SampleAStarSearchGraphWrapper> BacktrackAction;

      	//methods that astar needs:

      	//easy getters
      	virtual cost_type getCostForNode(const value_type& node) = 0;
      	virtual void setExpanded(value_type& node) = 0;
      	virtual bool isExpanded(value_type& node) = 0;
      	virtual bool isGoalNode(value_type& node) = 0;
      	virtual boost::optional<value_type> removeBestOpen() = 0;
      	virtual cost_type getCostBetween(value_type& parent, value_type& child) = 0;
      	virtual cost_type getHeuristic(const value_type& node) = 0;

      	//complicated "find best and return it"
      	virtual void updateCostSetOpen(value_type& node, cost_type cost) = 0;
      	virtual void updateCostSetOpen(value_type& node, cost_type newCosts, value_type& parentNode) = 0;

      	//functions astar gives a callback, that has to be called
      	virtual void forAllSuccessorNodes(value_type& node, std::function<void(value_type&)> action) = 0;
      	virtual void forAllPredecessorNodes(value_type& node, BacktrackAction& action) = 0;
      	virtual bool reOpenIf(value_type& node, std::function<bool(const value_type&)> action) = 0;

      };

	}
}

#endif // __AITOOLS_ASTAR_H_INCLUDED
