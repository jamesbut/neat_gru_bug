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

#ifndef __AITOOLS_RESTRICTED_VALUE_H_INCLUDED
#define __AITOOLS_RESTRICTED_VALUE_H_INCLUDED

#include <aiTools/Math/Numeric.h>
#include <aiTools/Math/Geometry.h>
#include <aiTools/Util/Helper.h>

#include <limits>

namespace aiTools
{
	/**
	 * Allows to create restricted values like Angles, Indices, normalized vectors and so on
	 * without the need to check.
	 */
	template<typename RestrictionType>
	struct RestrictedValue
	{
		RestrictedValue(const typename RestrictionType::value_type& value = typename RestrictionType::value_type()) : mValue(RestrictionType::apply(value))
		{}

		RestrictedValue& operator=(const typename RestrictionType::value_type& value)
		{
			mValue = RestrictionType::apply(value);
			return *this;
		}

		operator const typename RestrictionType::value_type&() const
		{
			return mValue;
		}

		typename RestrictionType::value_type mValue;
	};

	template<typename value_type, typename CorrectionLow>
	class UnsignedRestriction
	{
	public:
		static value_type apply(const value_type& value)
		{
			return CorrectionLow::correct(value);
		}
	};

	template<typename IntervalType, template <typename> class CorrectionType>
	class RangeRestriction
	{
	public:

		typedef typename IntervalType::value_type value_type;

		static typename IntervalType::value_type apply(const typename IntervalType::value_type& value)
		{
			return CorrectionType<IntervalType>::correct(value);
		}
	};

	template<typename T>
	class IsNotNullRestriction
	{
	public:

		typedef T value_type;

		static value_type apply(const value_type& value)
		{
			ASSERT(value != 0, "not null restricted value constructed from null value");
			return value;
		}
	};

	template<typename VectorType>
	class NormalizedRestriction
	{
	public:

		typedef VectorType value_type;

		static VectorType apply(const VectorType& value)
		{
			return value.getNormalized();
		}
	};

	template<typename T>
	class MinusPiToPi
	{
	public:

		typedef T value_type;

		static_assert(std::is_signed<value_type>::value, "the value type used for MinusPiToPi is unsigned!");

		static value_type getLowerBound()
		{
			return value_type(-Math::PI);
		}

		static value_type getUpperBound()
		{
			return value_type(Math::PI);
		}

		value_type lower() const
		{
			return getLowerBound();
		}

		value_type upper() const
		{
			return getUpperBound();
		}
	};

	template<typename T>
	class ZeroToTwoPi
	{
	public:

		typedef T value_type;

		static value_type getLowerBound()
		{
			return value_type(0);
		}

		static value_type getUpperBound()
		{
			return value_type(Math::TWO_PI);
		}

		value_type lower() const
		{
			return getLowerBound();
		}

		value_type upper() const
		{
			return getUpperBound();
		}
	};

	template<typename T>
	class Minus180To180
	{
	public:

		typedef T value_type;

		static_assert(std::is_signed<value_type>::value, "the value type used for Minus180To180 is unsigned!");

		static value_type getLowerBound()
		{
			return value_type(-180);
		}

		static value_type getUpperBound()
		{
			return value_type(180);
		}

		value_type lower() const
		{
			return getLowerBound();
		}

		value_type upper() const
		{
			return getUpperBound();
		}
	};

	template<typename T>
	class ZeroTo360
	{
	public:

		typedef T value_type;

		static value_type getLowerBound()
		{
			return value_type(0);
		}

		static value_type getUpperBound()
		{
			return value_type(360);
		}

		value_type lower() const
		{
			return getLowerBound();
		}

		value_type upper() const
		{
			return getUpperBound();
		}
	};

	template<typename T>
	class MinusOneToOne
	{
	public:

		typedef T value_type;

		static_assert(std::is_signed<value_type>::value, "the value type used for MinusOneToOne is unsigned!");

		static value_type getLowerBound()
		{
			return -1;
		}

		static value_type getUpperBound()
		{
			return 1;
		}

		value_type lower() const
		{
			return getLowerBound();
		}

		value_type upper() const
		{
			return getUpperBound();
		}
	};

	template<typename T>
	class PositiveRange
	{
	public:

		typedef T value_type;

		static value_type getLowerBound()
		{
			return 0;
		}

		static value_type getUpperBound()
		{
			if(std::numeric_limits<T>::has_infinity)
				return std::numeric_limits<T>::infinity();
			return std::numeric_limits<T>::max();
		}

		value_type lower() const
		{
			return getLowerBound();
		}

		value_type upper() const
		{
			return getUpperBound();
		}
	};

	template<typename IntervalType>
	class WrapAround
	{
	public:
		WrapAround(const IntervalType& interval) : mInterval(interval)
		{}

		static typename IntervalType::value_type correct(const typename IntervalType::value_type& value)
		{
			IntervalType interval;

			return correct_impl(interval, value);
		}

		static typename IntervalType::value_type correct_impl(const IntervalType& interval, const typename IntervalType::value_type& value)
		{
			assert_nan_and_inf(value);

			typename IntervalType::value_type result(value);
			const typename IntervalType::value_type lower = interval.lower();
			const typename IntervalType::value_type upper = interval.upper();
			const typename IntervalType::value_type difference = upper-lower;

			//TODO: use fmod here

			while(result<lower) result+=difference;
			while(result>=upper) result-=difference;

			return result;
		}

		typename IntervalType::value_type operator()(const typename IntervalType::value_type& value) const
		{
			return correct_impl(mInterval, value);
		}

	private:
		IntervalType mInterval;
	};

	template<typename IntervalType>
	class Clamp
	{
	public:
		Clamp(const IntervalType& interval) : mInterval(interval)
		{}

		static typename IntervalType::value_type correct(const typename IntervalType::value_type& value)
		{
			IntervalType interval;

			return correct_impl(interval, value);
		}

		static typename IntervalType::value_type correct_impl(const IntervalType& interval, const typename IntervalType::value_type& value)
		{
			const typename IntervalType::value_type lower = interval.lower();
			const typename IntervalType::value_type upper = interval.upper();

			if(value < lower)
				return lower;
			if(value > upper)
				return upper;
			return value;
		}

		typename IntervalType::value_type operator()(const typename IntervalType::value_type& value) const
		{
			return correct_impl(mInterval, value);
		}
	private:
		IntervalType mInterval;
	};

	template<typename IntervalType>
	class AssertIfOutsideRange
	{
	public:
		AssertIfOutsideRange(const IntervalType& interval) : mInterval(interval)
		{}

		static typename IntervalType::value_type correct(const typename IntervalType::value_type& value)
		{
			IntervalType interval;

			return correct_impl(interval, value);
		}

		static typename IntervalType::value_type correct_impl(const IntervalType& interval, const typename IntervalType::value_type& value)
		{
			const typename IntervalType::value_type lower = interval.lower();
			const typename IntervalType::value_type upper = interval.upper();

			ASSERT(value >= lower, "restricted value " << value << " not in range [" << lower << "," << upper << ")");
			ASSERT(value < upper, "restricted value " << value << " not in range [" << lower << "," << upper << ")");

			return value;
		}

		typename IntervalType::value_type operator()(const typename IntervalType::value_type& value) const
		{
			return correct_impl(mInterval, value);
		}
	private:
		IntervalType mInterval;
	};

	template<typename IntervalType>
	class Reflect
	{
	public:
		Reflect(const IntervalType& interval) : mInterval(interval)
		{}

		static typename IntervalType::value_type correct(const typename IntervalType::value_type& value)
		{
			IntervalType interval;

			return correct_impl(interval, value);
		}

		static typename IntervalType::value_type distToInterval(const IntervalType& interval, const typename IntervalType::value_type& value)
		{
			using T = typename IntervalType::value_type;
			const T lower = interval.lower();
			const T upper = interval.upper();

			if(value < lower)
				return lower - value;
			else if(value > upper)
				return value - upper;

			return 0;
		}

		static typename IntervalType::value_type correct_impl(const IntervalType& interval, const typename IntervalType::value_type& value)
		{
			using T = typename IntervalType::value_type;
			assert_nan_and_inf(value);

			const T lower = interval.lower();
			const T upper = interval.upper();
			const T intervalSize = (upper-lower);
			T distanceToInterval;
			T result{value};

			while((distanceToInterval = distToInterval(interval, result)) > 0)
			{
				if(result < lower)
					result = upper + (distanceToInterval - intervalSize);
				else if(result > upper)
					result = lower - (distanceToInterval - intervalSize);
			}

			return result;
		}

		typename IntervalType::value_type operator()(const typename IntervalType::value_type& value)  const
		{
			return correct_impl(mInterval, value);
		}

	private:
		IntervalType mInterval;
	};

	template<typename IntervalType>
	class Abs
	{
	public:
		static typename IntervalType::value_type correct(const typename IntervalType::value_type& value)
		{
			using std::abs;

			return abs(value);
		}
	};
}
#endif // __AITOOLS_RESTRICTED_VALUE_H_INCLUDED
