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
#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <ewoms/eclgrids/utility/stopwatch.hh>
#include <ewoms/eclio/errormacros.hh>

namespace Ewoms
{

    namespace time
    {
        using Duration = std::chrono::duration<double>;

	StopWatch::StopWatch()
	    : state_(State::UnStarted)
	{
	}

	void StopWatch::start()
	{
	    start_time_ = currentTime();
	    last_time_ = start_time_;
	    state_ = State::Running;
	}

	void StopWatch::stop()
	{
	    if (state_ != State::Running) {
		EWOMS_THROW(std::runtime_error, "Called stop() on a StopWatch that was not running.");
	    }
	    stop_time_ = currentTime();
	    state_ = State::Stopped;
	}

	double StopWatch::secsSinceLast()
	{
	    TimePoint run_time;
	    if (state_ == State::Running) {
		run_time = currentTime();
	    } else if (state_ == State::Stopped) {
		run_time = stop_time_;
	    } else {
		assert(state_ == State::UnStarted);
		EWOMS_THROW(std::runtime_error, "Called secsSinceLast() on a StopWatch that had not been started.");
	    }
	    Duration dur = run_time - last_time_;
	    last_time_ = run_time;
	    return dur.count();
	}

	double StopWatch::secsSinceStart()
	{
	    TimePoint run_time;
	    if (state_ == State::Running) {
		run_time = currentTime();
	    } else if (state_ == State::Stopped) {
		run_time = stop_time_;
	    } else {
		assert(state_ == State::UnStarted);
		EWOMS_THROW(std::runtime_error, "Called secsSinceStart() on a StopWatch that had not been started.");
	    }
	    Duration dur = run_time - start_time_;
	    return dur.count();
	}

        StopWatch::TimePoint StopWatch::currentTime() const
        {
            return std::chrono::high_resolution_clock::now();
        }

    } // namespace time

} // namespace Ewoms

