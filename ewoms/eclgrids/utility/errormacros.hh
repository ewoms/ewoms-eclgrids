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
#ifndef EWOMS_ECLGRIDS_ERRORMACROS_HH
#define EWOMS_ECLGRIDS_ERRORMACROS_HH

#if HAVE_EWOMS_COMMON
#include <ewoms/eclio/errormacros.hh>
#else

// reimplementation of macro when ewoms-common is not present

#include <string>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cassert>

// macros for reporting to stderr
#ifdef EWOMS_VERBOSE // Verbose mode
# include <iostream>
# define EWOMS_REPORT do { std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " } while (false)
# define EWOMS_MESSAGE(x) do { EWOMS_REPORT; std::cerr << x << "\n"; } while (false)
# define EWOMS_MESSAGE_IF(cond, m) do {if(cond) EWOMS_MESSAGE(m);} while (false)
#else // non-verbose mode (default)
# define EWOMS_REPORT do {} while (false)
# define EWOMS_MESSAGE(x) do {} while (false)
# define EWOMS_MESSAGE_IF(cond, m) do {} while (false)
#endif

// Macro to throw an exception. NOTE: For this macro to work, the
// exception class must exhibit a constructor with the signature
// (const std::string &message). Since this condition is not fulfilled
// for the std::exception, you should use this macro with some
// exception class derived from either std::logic_error or
// std::runtime_error.
//
// Usage: EWOMS_THROW(ExceptionClass, "Error message " << value);
#define EWOMS_THROW(Exception, message)                                   \
    do {                                                                \
        std::ostringstream oss__;                                       \
        oss__ << "[" << __FILE__ << ":" << __LINE__ << "] " << message; \
        throw Exception(oss__.str());                                   \
    } while (false)

// Same as EWOMS_THROW, except for not making an OpmLog::error() call.
//
// Usage: EWOMS_THROW_NOLOG(ExceptionClass, "Error message " << value);
#define EWOMS_THROW_NOLOG(Exception, message)                             \
    do {                                                                \
        std::ostringstream oss__;                                       \
        oss__ << "[" << __FILE__ << ":" << __LINE__ << "] " << message; \
        throw Exception(oss__.str());                                   \
    } while (false)

// throw an exception if a condition is true
#define EWOMS_ERROR_IF(condition, message) do {if(condition){ EWOMS_THROW(std::logic_error, message);}} while(false)

#endif // #if HAVE_EWOMS_COMMON

#endif // EWOMS_ERRORMACROS_HH
