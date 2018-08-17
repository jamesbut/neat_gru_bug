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
#ifndef __AITOOLS_TEST_H__INCLUDED
#define __AITOOLS_TEST_H__INCLUDED

//NOTE: this is not an official aiTools header. while it may provide interesting code, it is meant to be solely used by aiTool's own unit tests.

namespace aiTests
{
	template<typename T, template<typename, typename> class TestType, typename U, typename... Tail>
	struct holds_test;

	template<typename T, template<typename, typename> class TestType, typename U>
	struct holds_test<T, TestType, U>
	{
		static const bool value = TestType<T, U>::value;
	};

	template<typename T, template<typename, typename> class TestType, typename U, typename... Tail>
	struct holds_test
	{
		static_assert(TestType<T, U>::value, "Test failed for subtest...");
		static const bool value = TestType<T, U>::value && holds_test<T, TestType, Tail...>::value;
	};

	template<typename T, typename... ManyTypes>
	void testTypesForLosslesConversion()
	{
		static_assert(holds_test<T, aiTools::is_losslessly_convertible_to, ManyTypes...>::value, "type should be convertible to all given other types");
	}
}

#endif // __AITOOLS_TEST_H__INCLUDED
