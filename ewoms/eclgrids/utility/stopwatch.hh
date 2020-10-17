// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef EWOMS_STOPWATCH_HEADER
#define EWOMS_STOPWATCH_HEADER

#include <chrono>

namespace Ewoms
{

    namespace time
    {

	class StopWatch
	{
	public:
	    /// Default constructor. Before the StopWatch is start()-ed,
	    /// it is an error to call anything other than start().
	    StopWatch();

	    /// Starts the StopWatch. It is always legal to call
	    /// start(), even if not stop()-ped.
	    void start();
	    /// Stops the StopWatch. The watch no longer runs, until
	    /// restarted by a call to start().
	    void stop();

	    /// \return the number of running seconds that have passed
	    /// since last call to start(), secsSinceLast() or
	    /// secsSinceStart()
	    double secsSinceLast();
	    /// \return the number of running seconds that have passed
	    /// since last call to start().
	    double secsSinceStart();

	private:
	    enum class State { UnStarted, Running, Stopped };
            using TimePoint = std::chrono::high_resolution_clock::time_point;

            TimePoint currentTime() const;

	    State state_;
	    TimePoint start_time_;
	    TimePoint last_time_;
	    TimePoint stop_time_;
	};

    } // namespace time

} // namespace Ewoms

#endif // EWOMS_STOPWATCH_HEADER
