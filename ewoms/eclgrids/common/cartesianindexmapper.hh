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
#ifndef EWOMS_CARTESIANINDEXMAPPER_HEADER
#define EWOMS_CARTESIANINDEXMAPPER_HEADER

#include <array>
#include <cassert>

#include <dune/common/exceptions.hh>

namespace Dune
{
    /** \brief Interface class to access the logical Cartesian grid as used in industry
               standard simulator decks.
               */
    template< class Grid >
    class CartesianIndexMapper
    {
    public:
        /** \brief dimension of the grid */
        static const int dimension = Grid :: dimension ;

        /** \brief constructor taking grid */
        explicit CartesianIndexMapper( const Grid& )
        {
            DUNE_THROW(InvalidStateException,"CartesianIndexMapper not specialized for given grid");
        }

        /** \brief return Cartesian dimensions, i.e. number of cells in each direction  */
        const std::array<int, dimension>& cartesianDimensions() const
        {
            static std::array<int, dimension> a;
            return a;
        }

        /** \brief return total number of cells in the logical Cartesian grid */
        int cartesianSize() const
        {
            return 0;
        }

        /** \brief return number of cells in the active grid */
        int compressedSize() const
        {
            return 0;
        }

        /** \brief return index of the cells in the logical Cartesian grid */
        int cartesianIndex( const int /* compressedElementIndex */) const
        {
            return 0;
        }

        /** \brief return Cartesian coordinate, i.e. IJK, for a given cell */
        void cartesianCoordinate(const int /* compressedElementIndex */, std::array<int,dimension>& /* coords */) const
        {
        }
    };

} // end namespace Ewoms
#endif
