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

#ifndef __AITOOLS_VECTOR2_H_INCLUDED
#define __AITOOLS_VECTOR2_H_INCLUDED

#include <aiTools/Util/Helper.h>
#include <aiTools/Util/RestrictedValue.h>

#include <cmath>
#include <tuple>

namespace aiTools
{
	namespace Math
	{
		/**
		 * \brief A simple 2d-vector class
		 *
		 * Features overloaded plus and minus operators and a (squared)/length getter.
		 *
		 */
		template <typename T>
		struct Vector2
		{
			typedef T value_type;
			typedef T* iterator;
			typedef const T* const_iterator;
			const static std::size_t SIZE = 2;

			Vector2(T x_=T(0), T y_=T(0)) : x(x_), y(y_)
			{
			}

			Vector2(const T(&other)[SIZE]) : x(other[0]), y(other[1])
			{
			}

			Vector2(const std::tuple<T, T>& other) : x(std::get<0>(other)), y(std::get<1>(other))
			{
			}

			template <typename OtherType>
			explicit Vector2(const OtherType& other) : x(static_cast<T>(other[0])), y(static_cast<T>(other[1]))
			{
			}

			union
			{
				struct
				{
					T x;
					T y;
				};
				T data[2];
			};

			T lengthSquared() const
			{
				return x*x + y*y;
			}

			template <typename ReturnType=T>
			ReturnType length() const
			{
				using std::sqrt;

				T squaredLength = lengthSquared();
				ReturnType conv = static_cast<ReturnType>(squaredLength);


				return static_cast<ReturnType>(sqrt(conv));
			}

			void setLength(T l)
			{
				const T formerLength = length();
				ASSERT(formerLength != 0, "cannot scale vector of zero length");

				*this *= (l / formerLength);
			}

			void normalize()
			{
				setLength(1);
			}

			std::size_t size() const
			{
				return SIZE;
			}

			iterator begin()
			{
				return &data[0];
			}

			iterator end()
			{
				return &data[2];
			}

			const_iterator begin() const
			{
				return &data[0];
			}

			const_iterator end() const
			{
				return &data[2];
			}

			Vector2<T> getNormalized() const
			{
				Vector2<T> temp(*this);
				temp.normalize();
				return temp;
			}

			T& operator[](unsigned index)
			{
				return data[index];
			}

			const T& operator[](unsigned index) const
			{
				return data[index];
			}
		};

		template <typename T, typename U>
		Vector2<T>& operator-=(Vector2<T>& lhs, const Vector2<U>& rhs)
		{
			lhs.x -= rhs.x;
			lhs.y -= rhs.y;
			return lhs;
		}

		template <typename T, typename U>
		Vector2<T>& operator+=(Vector2<T>& lhs, const Vector2<U>& rhs)
		{
			lhs.x += rhs.x;
			lhs.y += rhs.y;
			return lhs;
		}

		template <typename T, typename U>
		Vector2<T>& operator*=(Vector2<T>& lhs, U rhs)
		{
			lhs.x *= rhs;
			lhs.y *= rhs;
			return lhs;
		}

		template <typename T, typename U>
		Vector2<T>& operator/=(Vector2<T>& lhs, U rhs)
		{
			U temp = 1/rhs;
			return lhs *= temp;
		}

		template <typename T, typename U>
		Vector2<T> operator-(const Vector2<T>& lhs, const Vector2<U>& rhs)
		{
			return Vector2<T>(lhs.x-rhs.x, lhs.y-rhs.y);
		}

		template <typename T, typename U>
		Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<U>& rhs)
		{
			return Vector2<T>(lhs.x+rhs.x, lhs.y+rhs.y);
		}

		template <typename T, typename U>
		Vector2<T> operator*(const Vector2<T>& lhs, U rhs)
		{
			return Vector2<T>(static_cast<T>(lhs.x*rhs), static_cast<T>(lhs.y*rhs));
		}

		template <typename T, typename U>
		Vector2<T> operator*(U lhs, const Vector2<T>& rhs)
		{
			return Vector2<T>(static_cast<T>(lhs*rhs.x), static_cast<T>(lhs*rhs.y));
		}

		template <typename T, typename U>
		Vector2<T> operator/(const Vector2<T>& lhs, U rhs)
		{
			U temp = 1/rhs;
			return lhs * temp;
		}

		template <typename T, typename U>
		bool operator==(const Vector2<T>& lhs, const Vector2<U>& rhs)
		{
			return lhs.x == rhs.x && lhs.y == rhs.y;
		}

		template <typename T, typename U>
		bool operator!=(const Vector2<T>& lhs, const Vector2<U>& rhs)
		{
			return !(lhs == rhs);
		}

		template <typename T>
		Vector2<T> operator-(const Vector2<T>& vec)
		{
			return Vector2<T>(-vec.x, -vec.y);
		}

		typedef Vector2<double> Vector2d;
		typedef Vector2<float> Vector2f;

		typedef RestrictedValue<NormalizedRestriction<Vector2d>> NormalizedVector2d;
		typedef RestrictedValue<NormalizedRestriction<Vector2f>> NormalizedVector2f;
	}
}

#endif // __AITOOLS_VECTOR2_H_INCLUDED
