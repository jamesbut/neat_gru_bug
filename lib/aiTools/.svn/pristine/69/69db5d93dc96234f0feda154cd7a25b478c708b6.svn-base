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

#ifndef __AITOOLS_STOPWATCH_H_INCLUDED
#define __AITOOLS_STOPWATCH_H_INCLUDED

#include <aiTools/Time/Time.h>
#include <aiTools/Time/WallClock.h>

namespace aiTools
{
	/**
	 * \brief A stopwatch.
	 *
	 * Use the start() and stop() methods to measure a time frame, returns average runtime.
	 *
	 */
	class StopWatch
	{
	public:
		StopWatch();

		/// returns the average runtime
		Time getAverageRuntime() const;

		/// returns the longest runtime
		Time getMaxRuntime() const;

		/// returns the total runtime
		Time getTotalRuntime() const;

		/// returns the numer of stop calls
		long long getNumberOfCalls() const;

		/// starts measuring time
		void start();

		/// stops, registers and returns the time passed since start()
		Time stop();

		/// resets all saved info
		void reset();

	private:
		long long mTimesCalled;
		Time mSummedRuntime;
		Time mMaxRuntime;
		Time mStarted;

		WallClock mClock;
	};

}

#endif //__AITOOLS_STOPWATCH_H_INCLUDED
