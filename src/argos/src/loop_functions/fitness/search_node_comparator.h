#ifndef _SEARCH_NODE_WRAPPER_H_
#define _SEARCH_NODE_WRAPPER_H_

#include <tuple>

struct SearchGridWrapper;
struct SearchNode;

class SearchNodeComparator {

   typedef std::tuple<long unsigned int, long unsigned int> index_type;

private:

	SearchGridWrapper& mSearchGraphWrapper;

public:

	SearchNodeComparator(SearchGridWrapper& search_wrapper) : mSearchGraphWrapper(search_wrapper) {}

	bool operator()(const index_type& n1, const index_type& n2);

};

#endif
