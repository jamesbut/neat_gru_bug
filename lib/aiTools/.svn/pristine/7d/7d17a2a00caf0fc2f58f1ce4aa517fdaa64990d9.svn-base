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
#ifndef __AITOOLS_ITERATE_MULTIPLE_H__INCLUDED
#define __AITOOLS_ITERATE_MULTIPLE_H__INCLUDED

#include <iterator>
#include <type_traits>

namespace aiTools
{
	namespace detail
	{
		struct stop_recursion{};

		template <typename Action, typename... Params>
		bool for_each_multiple_impl(stop_recursion, Action action, Params&&... params)
		{
			action(std::forward<Params>(params)...);
			return true;
		}

		template <typename ForwardIterator, typename... OtherParams>
		typename std::enable_if<std::is_base_of<std::forward_iterator_tag, typename ForwardIterator::iterator_category>::value, bool>::type
		for_each_multiple_impl(ForwardIterator& first, const ForwardIterator& last, OtherParams&&... params)
		{
			if(first == last)
				return false;

			bool othersReturn = for_each_multiple_impl(std::forward<OtherParams>(params)..., *first);
			++first;

			return true && othersReturn;
		}
	}

	///as std::for_each, just that it iterates over multiple containers the same time and calls the action passed with the current element of each container. unfortunately, expects the action first, then the iterators.
	template <typename... Iters, typename Action>
	Action for_each_multiple(Action action, Iters... iters) //TODO enhance with a recursive decltype-determining function... get the last param of iters, -> this is our return type - then we can rearrange the order and have action last.
	{
		//note that iters are taken by copy. we are referencing them from now on, and need a clean copy to work on.
		bool succeeded = true;
		auto recursionStopper = detail::stop_recursion();

		while(succeeded)
			succeeded = detail::for_each_multiple_impl(iters..., recursionStopper, action);

		return std::move(action);
	}
}

#endif // __AITOOLS_ITERATE_MULTIPLE_H__INCLUDED
