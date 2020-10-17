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
#ifndef EWOMS_POLYHEDRALCARTESIANINDEXMAPPER_HEADER
#define EWOMS_POLYHEDRALCARTESIANINDEXMAPPER_HEADER

#include <ewoms/eclgrids/common/cartesianindexmapper.hh>
#include <ewoms/eclgrids/polyhedralgrid.hh>

namespace Dune
{
    template< int dim, int dimworld, typename coord_t >
    class CartesianIndexMapper< PolyhedralGrid< dim, dimworld, coord_t > >
    {
        typedef PolyhedralGrid< dim, dimworld, coord_t >  Grid;

        const Grid& grid_;
        const int cartesianSize_;

        int computeCartesianSize() const
        {
            int size = cartesianDimensions()[ 0 ];
            for( int d=1; d<dim; ++d )
                size *= cartesianDimensions()[ d ];
            return size ;
        }
    public:
        static const int dimension = Grid :: dimension ;

        explicit CartesianIndexMapper( const Grid& grid )
          : grid_( grid ),
            cartesianSize_( computeCartesianSize() )
        {}

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
            return grid_.size( 0 );
        }

        int cartesianIndex( const int compressedElementIndex ) const
        {
            assert( compressedElementIndex >= 0 && compressedElementIndex < compressedSize() );
            return grid_.globalCell()[ compressedElementIndex ];
        }

        void cartesianCoordinate(const int compressedElementIndex, std::array<int,dimension>& coords) const
        {
          int gc = cartesianIndex( compressedElementIndex );
          if( dimension >=2 )
          {
              for( int d=0; d<dimension-2; ++d )
              {
                coords[d] = gc % cartesianDimensions()[d];  gc /= cartesianDimensions()[d];
              }

              coords[dimension-2] = gc % cartesianDimensions()[dimension-2];
              coords[dimension-1] = gc / cartesianDimensions()[dimension-1];
          }
          else
              coords[ 0 ] = gc ;
        }
    };

} // end namespace Ewoms
#endif
