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

#ifndef __AITOOLS_PLATFORM_DETECTION_H_INCLUDED
#define __AITOOLS_PLATFORM_DETECTION_H_INCLUDED

#define __AITOOLS_OS_LINUX_VALUE 0
#define __AITOOLS_OS_WINDOWS_VALUE 1

#define __AITOOLS_COMPILER_GCC_VALUE 0
#define __AITOOLS_COMPILER_MSVC_VALUE 1

///needless to say, this is an extremely crude implementation which does not detect most combinations

#if !defined(__AITOOLS_OS)
	#ifdef _MSC_VER
		#define __AITOOLS_OS __AITOOLS_OS_WINDOWS_VALUE
		#define __AITOOLS_OS_WINDOWS __AITOOLS_OS_WINDOWS_VALUE
	#else
		#define __AITOOLS_OS __AITOOLS_OS_LINUX_VALUE
		#define __AITOOLS_OS_LINUX __AITOOLS_OS_LINUX_VALUE
	#endif
#endif

#if !defined(__AITOOLS_COMPILER)
	#ifdef _MSC_VER
		#define __AITOOLS_COMPILER __AITOOLS_COMPILER_MSVC_VALUE
		#define __AITOOLS_COMPILER_MSVC __AITOOLS_COMPILER_MSVC_VALUE
	#else
		#define __AITOOLS_COMPILER __AITOOLS_COMPILER_GCC_VALUE
		#define __AITOOLS_COMPILER_GCC __AITOOLS_COMPILER_GCC_VALUE
	#endif
#endif


#endif // __AITOOLS_PLATFORM_DETECTION_H_INCLUDED
