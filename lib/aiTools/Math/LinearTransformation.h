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

#ifndef __AITOOLS_LINEAR_TRANSFORMATION_H_INCLUDED
#define __AITOOLS_LINEAR_TRANSFORMATION_H_INCLUDED

#include <aiTools/Util/Range.h>

namespace aiTools
{
	namespace Math
	{
		/// helper class to scale values linearly, stating from mTranslate (second param), rising by mScale for every +1 passed to operator()
		template<typename T>
		struct LinearTransformation
		{
			typedef T value_type;

			/// creates a linear transformation of the form f(x) = a*x + b
			LinearTransformation(const T& a, const T& b) : mScale(a), mTranslate(b)
			{}

			value_type operator()(const value_type& value) const
			{
				return (value * mScale) + mTranslate;
			}
		private:
			const T mScale;
			const T mTranslate;
		};

		/// returns a transformation that transforms between an input and an output range - input.lower is mapped to output.lower, upper to upper and everything inbetween scaled linearly
		template <typename T>
		LinearTransformation<T> transformBetweenRanges(const Range<T>& input, const Range<T>& output)
		{
			T slope = (output.mUpper-output.mLower) / (input.mUpper-input.mLower);
			T offset = output.mLower - (slope*input.mLower);

			return LinearTransformation<T>(slope, offset);
		}

		/// helper class to scale between two values lower and upper. feeding 0 into operator() yields lower, 1 upper - interpolated between. larger or smaller values extrapolate.
		template<typename T>
		struct LinearInterpolation
		{
			typedef T value_type;

			LinearInterpolation(const value_type& lower, const value_type& upper) : mLowerBound(lower), mDifference(upper - lower)
			{}

			LinearInterpolation(const Range<value_type>& bounds) : mLowerBound(bounds.mLower), mDifference(bounds.mUpper - bounds.mLower)
			{}

			LinearInterpolation(const T(&bounds)[2]) : mLowerBound(bounds[0]), mDifference(bounds[1] - bounds[0])
			{}
	
			template <typename V>
			value_type operator()(const V& value) const
			{
				return static_cast<value_type>((value * mDifference) + mLowerBound);
			}
		private:
			const value_type mLowerBound;
			const value_type mDifference;
		};
	}
}

#endif //__AITOOLS_LINEAR_TRANSFORMATION_H_INCLUDED
