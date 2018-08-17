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

#ifndef __AITOOLS_QUATERNION_H_INCLUDED
#define __AITOOLS_QUATERNION_H_INCLUDED

#include <aiTools/Math/Vector3.h>
#include <cmath>

namespace aiTools
{
	namespace Math
	{
		/**
		 * \brief A simple quaternion class
		 *
		 * Features some overloaded operators
		 *
		 */
		template <typename T>
		struct Quaternion
		{
			typedef T value_type;

			Quaternion(T x_=T(0), T y_=T(0), T z_=T(0), T w_=T(1)) : x(x_), y(y_), z(z_), w(w_)
			{
			}

			Quaternion(const T(&other)[4]) : x(other[0]), y(other[1]), z(other[2]), w(other[3])
			{
			}

			Quaternion(T angle, const Vector3<T>& axis) : //TODO figure out how to use "using" in initializer list...
				x(axis.x * std::sin(angle / 2.f)),
				y(axis.y * std::sin(angle / 2.f)),
				z(axis.z * std::sin(angle / 2.f)),
				w(std::cos(angle / 2.0f))
			{
			}

			void normalize()
			{
				*this = getNormalized();
			}

			void invert()
			{
				*this = getInverse();
			}

			T dot(const Quaternion<T>& other) const
			{
				return x * other.x + y * other.y + z * other.z + w * other.w;
			}

			T lengthSquared() const
			{
				return dot(*this);
			}

			template <typename ReturnType=T>
			ReturnType length() const
			{
				using std::sqrt;

				T squaredLength = lengthSquared();
				ReturnType conv = static_cast<ReturnType>(squaredLength);

				return static_cast<ReturnType>(sqrt(conv));
			}

			Quaternion<T> getInverse() const
			{
				return Quaternion<T>(-x, -y, -z, w);
			}

			Quaternion<T> getNormalized() const
			{
				using std::sqrt;

				T squaredLength = lengthSquared();
				ASSERT(squaredLength != 0, "cannot normalize zero length quaternion");

				T norm = sqrt(squaredLength	);

				return Quaternion<T>(x/norm, y/norm, z/norm, w/norm);
			}

			template <typename U>
			Vector3<U> rotate(const Vector3<U>& vec)
			{
				Quaternion helper = *this * vec;
				helper *= getInverse();

				return Vector3<U>(helper.x, helper.y, helper.z);
			}

			T& operator[](unsigned index)
			{
				return data[index];
			}

			const T& operator[](unsigned index) const
			{
				return data[index];
			}

			Quaternion<T>& operator*=(const Quaternion<T>& other)
			{
				*this = *this * other;
				return *this;
			}

			union
			{
				struct
				{
					T x;
					T y;
					T z;
					T w;
				};
				T data[4];
			};
		};

		template <typename T, typename U>
		Quaternion<T> operator*(const Quaternion<T>& quat, const Vector3<U>& vec)
		{
			return Quaternion<T>(quat.w * vec.x + quat.y * vec.z - quat.z * vec.y,
							  quat.w * vec.y + quat.z * vec.x - quat.x * vec.z,
							  quat.w * vec.z + quat.x * vec.y - quat.y * vec.x,
							 -quat.x * vec.x - quat.y * vec.y - quat.z * vec.z);
		}

		template <typename T>
		Quaternion<T> operator*(const Quaternion<T>& quat, const Quaternion<T>& other)
		{
			Quaternion<T> temp;

			temp.x = quat.w * other.x + quat.x * other.w + quat.y * other.z - quat.z * other.y;
			temp.y = quat.w * other.y + quat.y * other.w + quat.z * other.x - quat.x * other.z;
			temp.z = quat.w * other.z + quat.z * other.w + quat.x * other.y - quat.y * other.x;
			temp.w = quat.w * other.w - quat.x * other.x - quat.y * other.y - quat.z * other.z;

			return temp;
		}
	}
}

#endif // __AITOOLS_QUATERNION_H_INCLUDED
