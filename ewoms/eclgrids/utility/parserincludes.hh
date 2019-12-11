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
#ifndef EWOMS_ECLGRIDS_PARSER_INCLUDES_HH
#define EWOMS_ECLGRIDS_PARSER_INCLUDES_HH

#if HAVE_ECL_INPUT
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellconnections.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>
#include <ewoms/eclio/utility/activegridcells.hh>

namespace Dune {
    namespace cpgrid {
        typedef Ewoms::Well EwomsEclWellType;
        typedef Ewoms::EclipseState EwomsEclEclipseStateType;
    }
}
#else // #if HAVE_ECL_INPUT

namespace Dune {
    namespace cpgrid {
        typedef int EwomsEclWellType;
        typedef int EwomsEclEclipseStateType;
    }
}

#endif // #if HAVE_ECL_INPUT

#endif // #ifndef EWOMS_ECLGRIDS_PARSER_INCLUDES_HH
