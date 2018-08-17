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

#ifndef __AITOOLS_MOVING_AVERAGE_H_INCLUDED
#define __AITOOLS_MOVING_AVERAGE_H_INCLUDED

#include <aiTools/Math/Statistics.h>
#include <aiTools/Math/Vector.h>

#include <vector>

namespace aiTools
{
	namespace Math
	{
		//TODO: these detail classes should be elsewhere
		template <typename T>
		struct AddVectors
		{
			T operator ()(const T& p1, const T& p2) const
			{
				const std::size_t s = p1.size();
				ASSERT(p1.size() == p2.size(), "vectors not of equal size");

				T result(s, typename T::value_type());

				for(std::size_t i = 0;i<s; ++i)
				{
					result[i] = p1[i] + p2[i];
				}
				return result;
			}

			T operator ()(const T& p1, const typename T::value_type & p2) const
			{
				const std::size_t s = p1.size();
				T result(s, typename T::value_type());

				for(std::size_t i = 0;i<s; ++i)
				{
					result[i] = p1[i] + p2;
				}
				return result;
			}
		};

		template <typename T>
		struct DivVectors
		{
			T operator ()(const T& p1, const T& p2) const
			{
				const std::size_t s = p1.size();
				ASSERT(p1.size() == p2.size(), "vectors not of equal size");

				T result(s, typename T::value_type());

				for(std::size_t i = 0;i<s; ++i)
				{
					result[i] = p1[i] / p2[i];
				}
				return result;
			}

			T operator ()(const T& p1, const typename T::value_type & p2) const
			{
				const std::size_t s = p1.size();
				T result(s, typename T::value_type());

				for(std::size_t i = 0;i<s; ++i)
				{
					result[i] = p1[i] / p2;
				}
				return result;
			}
		};

		template <typename T>
		struct MultiplyVectors
		{
			T operator ()(const T& p1, const T& p2) const
			{
				const std::size_t s = p1.size();
				ASSERT(p1.size() == p2.size(), "vectors not of equal size");

				T result(s, typename T::value_type());

				for(std::size_t i = 0;i<s; ++i)
				{
					result[i] = p1[i] * p2[i];
				}
				return result;
			}

			T operator ()(const T& p1, const typename T::value_type & p2) const
			{
				const std::size_t s = p1.size();
				T result(s, typename T::value_type());

				for(std::size_t i = 0;i<s; ++i)
				{
					result[i] = p1[i] * p2;
				}
				return result;
			}
		};



		/// one-time used proof of concept, usage discouraged
		template <typename T>
		class MovingAverage
		{
		public:
			MovingAverage(unsigned int size, const T& default_value = T())
				: mData(size, default_value),
				  mPos(0)
			{}

			void insert(const T& new_value)
			{
				mData[mPos++] = new_value;

				if(mPos >= mData.size())
					mPos = 0;
			}

			T getAverage() const
			{
				T avg = average<std::vector<T>, T, AddVectors, DivVectors>(mData);
				normalize(avg);

				return avg;
			}

			T getSum() const
			{
				T s = mData[0];

				for(unsigned int i=1;i<mData.size();++i)
				{
					for(unsigned int j=0;j<mData[i].size();++j)
					{
						s[j] += mData[i][j];
					}
				}

				return s;
			}


		private:
			std::vector<T> mData;
			std::size_t mPos;
		};

	}
}

#endif // __AITOOLS_MOVING_AVERAGE_H_INCLUDED
