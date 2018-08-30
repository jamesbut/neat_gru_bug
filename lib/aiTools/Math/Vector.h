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

#ifndef __AITOOLS_VECTOR_H_INCLUDED
#define __AITOOLS_VECTOR_H_INCLUDED

#include <aiTools/Util/MetaprogrammingHelpers.h>
#include <aiTools/Util/Functors.h>
#include <aiTools/Math/Distance.h>

#include <initializer_list>
#include <functional>
#include <numeric>

namespace aiTools
{
	namespace Math
	{

		/**
		 * \brief A simple n-d-vector class. Do not use for gigantic arrays, as data ist held on the stack. use std::vector instead for that purpose.
		 *
		 * Features overloaded plus and minus operators and a (squared)/length getter.
		 *
		 */
		template <typename T, unsigned int C>
		struct Vector
		{
			typedef T value_type;
			typedef T* iterator;
			typedef const T* const_iterator;
			const static std::size_t SIZE = C;

			Vector()
			{
				for(std::size_t i=0; i<C; ++i)
				{
					data[i] = T(0);
				}
			}

			Vector(const Vector& other) = default;

			template <typename OtherType>
			explicit Vector(const OtherType& other)
			{
				for(std::size_t i=0; i<C; ++i)
				{
					data[i] = static_cast<T>(other[i]);
				}
			}

			Vector(const T(&other)[C])
			{
				for(std::size_t i=0; i<C; ++i)
				{
					data[i] = static_cast<T>(other[i]);
				}
			}

			//TODO tuple overload...?

			template<typename OtherType, std::size_t OtherSize>
			Vector& operator=(const OtherType(&other)[OtherSize])
			{
				static_assert(OtherSize == C, "sizes do not match");
				for(std::size_t i=0; i<C; ++i)
				{
					data[i] = static_cast<T>(other[i]);
				}
				return *this;
			}

			explicit Vector(do_not_initialize)
			{
			}

			template <typename FwdIter>
			explicit Vector(FwdIter obegin, FwdIter oend)
			{
				iterator begin = begin();
				iterator end = end();

				while(obegin != oend && begin != end)
				{
					*begin++ = *obegin++;
				}
			}

			T data[C];

			iterator begin()
			{
				return &data[0];
			}

			iterator end()
			{
				return &data[C];
			}

			const_iterator begin() const
			{
				return &data[0];
			}

			const_iterator end() const
			{
				return &data[C];
			}


			T lengthSquared() const
			{
				return euclidean_length_squared(begin(), end());
			}

			T length() const
			{
				return euclidean_length(begin(), end());
			}

			template <typename ReturnType=T>
			ReturnType length() const
			{
				return euclidean_length<const_iterator, ReturnType>(begin(), end());
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

			Vector<T, C> getNormalized() const
			{
				Vector<T, C> temp(*this);
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

		private:
			void init(std::size_t pos, unsigned lastItem)
			{
				data[pos]=lastItem;
			}

			template<typename ... V>
			void init(std::size_t pos, unsigned currentItem, V ... args)
			{
				data[pos]=currentItem;
				init(pos+1, args...);
			}
		};

		template <typename T, typename U, unsigned int C>
		Vector<T, C>& operator-=(Vector<T, C>& lhs, const Vector<U, C>& rhs)
		{
			std::minus<T> modifier;
			std::transform(lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), modifier);
			return lhs;
		}

		template <typename T, typename U, unsigned int C>
		Vector<T, C>& operator+=(Vector<T, C>& lhs, const Vector<U, C>& rhs)
		{
			std::plus<T> modifier;
			std::transform(lhs.begin(), lhs.end(), rhs.begin(), lhs.begin(), modifier);
			return lhs;
		}

		template <typename T, typename U, unsigned int C>
		Vector<T, C>& operator*=(Vector<T, C>& lhs, const U& rhs)
		{
			ModifyWith<std::multiplies<T>
					, U> modifier(rhs);
			std::transform(lhs.begin(), lhs.end(), lhs.begin(), modifier);
			return lhs;
		}

		template <typename T, typename U, unsigned int C>
		Vector<T, C>& operator/=(Vector<T, C>& lhs, const U& rhs)
		{
			ModifyWith<std::divides<T>, U> modifier(rhs);
			std::transform(lhs.begin(), lhs.end(), lhs.begin(), modifier);
			return lhs;
		}

		template <typename T, typename U, unsigned int C>
		Vector<T, C> operator-(const Vector<T, C>& lhs, const Vector<U, C>& rhs)
		{
			std::minus<T> modifier;

			Vector<T, C> result(do_not_initialize);
			std::transform(lhs.begin(), lhs.end(), rhs.begin(), result.begin(), modifier);
			return result;
		}

		template <typename T, typename U, unsigned int C>
		Vector<T, C> operator+(const Vector<T, C>& lhs, const Vector<U, C>& rhs)
		{
			std::plus<T> modifier;

			Vector<T, C> result(do_not_initialize);
			std::transform(lhs.begin(), lhs.end(), rhs.begin(), result.begin(), modifier);
			return result;
		}

		template <typename T, typename U, unsigned int C>
		Vector<T, C> operator*(const Vector<T, C>& lhs, const U& rhs)
		{
			ModifyWith<std::multiplies<T>, U> modifier(rhs);

			Vector<T, C> result(do_not_initialize);
			std::transform(lhs.begin(), lhs.end(), lhs.begin(), modifier);
			return result;
		}

		template <typename T, typename U, unsigned int C>
		Vector<T, C> operator*(const U& rhs, const Vector<T, C>& lhs)
		{
			ModifyWith<std::multiplies<T>, U> modifier(rhs);

			Vector<T, C> result(do_not_initialize);
			std::transform(lhs.begin(), lhs.end(), lhs.begin(), modifier);
			return result;
		}

		template <typename T, typename U, unsigned int C>
		Vector<T, C> operator/(const Vector<T, C>& lhs, const U& rhs)
		{
			ModifyWith<std::divides<T>, U> modifier(rhs);

			Vector<T, C> result(do_not_initialize);
			std::transform(lhs.begin(), lhs.end(), lhs.begin(), modifier);
			return result;
		}

		template <typename T, typename U, unsigned int C>
		bool operator==(const Vector<T, C>& lhs, const Vector<U, C>& rhs)
		{
			return std::equal(lhs.begin(), lhs.end(), rhs.end());
		}

		template <typename T, typename U, unsigned int C>
		bool operator!=(const Vector<T, C>& lhs, const Vector<U, C>& rhs)
		{
			return !std::equal(lhs.begin(), lhs.end(), rhs.end());
		}


		template <typename T, unsigned int C>
		Vector<T, C> operator-(const Vector<T, C>& lhs)
		{
			std::negate<T> modifier;

			Vector<T, C> result(do_not_initialize);
			std::transform(lhs.begin(), lhs.end(), lhs.begin(), modifier);
			return result;
		}

		template <typename FwdIterType, typename OtherFwdIterType, typename ResultType = double>
		typename std::enable_if<!is_pair<typename FwdIterType::value_type>::value, ResultType>::type
		dot_product(FwdIterType beginA, FwdIterType endA, OtherFwdIterType beginB, OtherFwdIterType endB)
		{
			//ensure equal type in both containers
			static_assert(std::is_same<typename FwdIterType::value_type, typename OtherFwdIterType::value_type>::value, "dot product should only be called on containers containing the same type.");
			return std::inner_product(beginA, endA, beginB, ResultType(0));
		}

		template <typename T, unsigned int C, typename ResultType = double>
		ResultType dot_product(const Vector<T, C>& lhs, const Vector<T, C>& rhs)
		{
			return dot_product(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
		}

		template <typename FwdIterType, typename OtherFwdIterType, typename OutputIterType>
		void piecewise_multiplication(FwdIterType beginA, FwdIterType endA, OtherFwdIterType beginB, OutputIterType beginC)
		{
			//ensure equal type in both containers
			static_assert(std::is_same<typename FwdIterType::value_type, typename OtherFwdIterType::value_type>::value, "piecewise multiplication should only be called on containers containing the same type.");

			std::multiplies<typename FwdIterType::value_type> modifier();
			std::transform(beginA, endA, beginB, beginC, modifier);
		}

		template <typename T, unsigned int C>
		Vector<T, C> piecewise_multiplication(const Vector<T, C>& lhs, const Vector<T, C>& rhs)
		{
			Vector<T, C> result(do_not_initialize);
			piecewise_multiplication(lhs.begin(), lhs.end(), rhs.begin(), result.begin());
			return result;
		}
	}
}

#endif // __AITOOLS_VECTOR_H_INCLUDED