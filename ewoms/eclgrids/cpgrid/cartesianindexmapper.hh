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
#ifndef EWOMS_CPGRIDCARTESIANINDEXMAPPER_HEADER
#define EWOMS_CPGRIDCARTESIANINDEXMAPPER_HEADER

#include <array>
#include <cassert>

#include <ewoms/eclgrids/common/cartesianindexmapper.hh>
#include <ewoms/eclgrids/cpgrid.hh>

namespace Dune
{
    template<>
    class CartesianIndexMapper< CpGrid >
    {
    public:
        static const int dimension = 3 ;
    protected:
        typedef CpGrid Grid;
        const Grid& grid_;
        const int cartesianSize_;

        int computeCartesianSize() const
        {
            int size = cartesianDimensions()[ 0 ];
            for( int d=1; d<dimension; ++d )
                size *= cartesianDimensions()[ d ];
            return size ;
        }

    public:
        explicit CartesianIndexMapper( const Grid& grid )
            : grid_( grid ),
              cartesianSize_( computeCartesianSize() )
        {
        }

        const std::array<int, dimension>& cartesianDimensions() const
        {
            return grid_.logicalCartesianSize();
        }

        int cartesianSize() const
        {
            return cartesianSize_;
        }

        int compressedSize() const
        {
            return grid_.globalCell().size();
        }

        int cartesianIndex( const int compressedElementIndex ) const
        {
            assert(  compressedElementIndex >= 0 && compressedElementIndex < compressedSize() );
            return grid_.globalCell()[ compressedElementIndex ];
        }

        void cartesianCoordinate(const int compressedElementIndex, std::array<int,dimension>& coords) const
        {
            grid_.getIJK( compressedElementIndex, coords );
        }
    };

} // end namespace Ewoms
#endif
