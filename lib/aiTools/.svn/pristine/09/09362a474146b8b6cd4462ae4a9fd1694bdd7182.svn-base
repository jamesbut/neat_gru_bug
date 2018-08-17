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

#ifndef __AITOOLS_NUMERIC_H_INCLUDED
#define __AITOOLS_NUMERIC_H_INCLUDED

#include <aiTools/Util/PlatformDetection.h>

#if __AITOOLS_COMPILER != __AITOOLS_COMPILER_MSVC_VALUE
#include <cmath>
#else
#include <float.h>
#endif

///This whole file is an ugly workaround for VS2012 not having a proper isnan and isinf

namespace aiTools
{

#if __AITOOLS_COMPILER != __AITOOLS_COMPILER_MSVC_VALUE
	using std::isnan;
	using std::isinf;
#else
	
	inline bool isnan(double val)
	{
		return _isnan(val) == 1;
	}

	inline bool isnan(float val)
	{
		return _isnan(val) == 1;
	}

	inline bool isinf(double val)
	{
		return _fpclass(val) == _FPCLASS_PINF;
	}

	inline bool isinf(float val)
	{
		return _fpclass(val) == _FPCLASS_PINF;
	}

#endif
}


#endif // __AITOOLS_NUMERIC_H_INCLUDED
