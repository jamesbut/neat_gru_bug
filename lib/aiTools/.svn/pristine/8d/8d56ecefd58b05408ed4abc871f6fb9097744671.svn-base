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

#ifndef __AITOOLS_LOSSLESS_CONVERSION_H_INCLUDED
#define __AITOOLS_LOSSLESS_CONVERSION_H_INCLUDED

namespace aiTools
{
	namespace detail
	{
		template <typename T>
		struct wrap_type
		{
			typedef T type;
		};


		template <typename T>
		struct make_signed_if_available
		{
			typedef typename std::conditional<!std::is_same<T, bool>::value && (std::is_integral<T>::value || std::is_enum<T>::value),
									std::make_signed<T>,
									wrap_type<T>>::type::type type;
		};

		template <typename T, typename U>
		struct is_losslessly_convertible_to_impl
		{
		   static const bool value = false;
		};

		//conversion to itself:
		template <typename T>
		struct is_losslessly_convertible_to_impl<T, T>
		{
		   static const bool value = true;
		};

		//conversion to other types:


		//bool
		template <>	struct is_losslessly_convertible_to_impl<bool, signed char>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<bool, signed short>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<bool, signed int>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<bool, signed long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<bool, signed long long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<bool, float>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<bool, double>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<bool, long double>{static const bool value = true;};


		//char
		template <>	struct is_losslessly_convertible_to_impl<signed char, signed short>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed char, signed int>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed char, signed long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed char, signed long long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed char, float>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed char, double>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed char, long double>{static const bool value = true;};

		//short
		template <>	struct is_losslessly_convertible_to_impl<signed short, signed int>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed short, signed long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed short, signed long long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed short, float>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed short, double>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed short, long double>{static const bool value = true;};

		//int
		template <>	struct is_losslessly_convertible_to_impl<signed int, signed long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed int, signed long long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed int, double>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed int, long double>{static const bool value = true;};

		//long
		template <>	struct is_losslessly_convertible_to_impl<signed long, signed long long>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<signed long, double>{static const bool value = sizeof(long) < 52;}; //double has 52 bits of resolution
		template <>	struct is_losslessly_convertible_to_impl<signed long, long double>{static const bool value = true;};

		//long long
		template <>	struct is_losslessly_convertible_to_impl<signed long long, long double>{static const bool value = true;};

		//float
		template <>	struct is_losslessly_convertible_to_impl<float, double>{static const bool value = true;};
		template <>	struct is_losslessly_convertible_to_impl<float, long double>{static const bool value = true;};

		//double
		template <>	struct is_losslessly_convertible_to_impl<double, long double>{static const bool value = true;};
	}

	//removes unsigned-ness, makes everything signed (if "unsigned" attib exists)
	template <typename T, typename U>
	struct is_losslessly_convertible_to
	{
	   static const bool value = detail::is_losslessly_convertible_to_impl<typename detail::make_signed_if_available<typename std::decay<T>::type>::type, typename detail::make_signed_if_available<typename std::decay<U>::type>::type>::value;
	};

}
#endif // __AITOOLS_LOSSLESS_CONVERSION_H_INCLUDED
