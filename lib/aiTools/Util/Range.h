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

#ifndef __AITOOLS_RANGE_H_INCLUDED
#define __AITOOLS_RANGE_H_INCLUDED

#include <aiTools/Util/Helper.h>
namespace aiTools
{
	/**
	 * A range [a,b), if a <=b.
	 * if a > b, ]a, b( is included.
	 */
	template <typename T>
	struct Range
	{
		typedef T value_type;

		Range(T l, T u) :
			mLower(l), mUpper(u)
		{
		}

		Range(const T(&other)[2]) :
			mLower(other[0]), mUpper(other[1])
		{
		}

		bool isInside(T value) const
		{
			if(mLower <= mUpper)
				return value >= mLower && value < mUpper;
			else
				return value >= mLower || value < mUpper;
		}

		value_type lower() const
		{
			return mLower;
		}

		value_type upper() const
		{
			return mUpper;
		}

		const T mLower;
		const T mUpper;
	};
}

#endif // __AITOOLS_RANGE_H_INCLUDED
