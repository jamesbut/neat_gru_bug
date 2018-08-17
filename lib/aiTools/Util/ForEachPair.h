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
#ifndef __AITOOLS_FOR_EACH_PAIR_H__INCLUDED
#define __AITOOLS_FOR_EACH_PAIR_H__INCLUDED

#include <iterator>

namespace aiTools
{
	///iterates over a container, calls an arbitrary action with each 2 consecutive (but not overlapping) elements, {1, 2, 3, 4} -> {1, 2}, {3, 4}
	template <typename ForwardIterator, typename Action>
	typename std::enable_if<std::is_base_of<std::forward_iterator_tag, typename ForwardIterator::iterator_category>::value, Action>::type
	for_each_pair(ForwardIterator first, ForwardIterator last, Action action)
	{
		while(first != last)
		{
			ForwardIterator second{first};
			++second;

			if(second != last)
				action(*first, *second);
			else
				break;

			first = ++second;
		}

		return std::move(action);
	}

	///iterates over a container, calls an arbitrary action with each 2 consecutive (including overlapping) elements, {1, 2, 3, 4} -> {1, 2}, {2, 3}, {3, 4}
	template <typename ForwardIterator, typename Action>
	typename std::enable_if<std::is_base_of<std::forward_iterator_tag, typename ForwardIterator::iterator_category>::value, Action>::type
	for_each_overlapping_pair(ForwardIterator first, ForwardIterator last, Action action)
	{
		if(first != last)
		{
			ForwardIterator second{first};
			++second;

			while(second != last)
			{
				action(*first, *second);

				first = second;
				++second;
			}
		}

		return std::move(action);
	}

}

#endif // __AITOOLS_FOR_EACH_PAIR_H__INCLUDED
