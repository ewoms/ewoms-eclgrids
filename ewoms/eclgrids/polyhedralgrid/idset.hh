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
#ifndef DUNE_POLYHEDRALGRID_IDSET_HH
#define DUNE_POLYHEDRALGRID_IDSET_HH

#include <dune/grid/common/indexidset.hh>

namespace Dune
{

  // PolyhedralGridIdSet
  // -------------------

  template< int dim, int dimworld, typename coord_t >
  class PolyhedralGridIdSet
      : public IdSet< PolyhedralGrid< dim, dimworld, coord_t >, PolyhedralGridIdSet< dim, dimworld, coord_t >, std::size_t /*IdType=size_t*/ >
  {
  public:
    typedef PolyhedralGrid<  dim, dimworld, coord_t > Grid;
    typedef typename std::remove_const< Grid >::type::Traits Traits;
    typedef std::size_t IdType;

    typedef PolyhedralGridIdSet< dim, dimworld, coord_t > This;
    typedef IdSet< Grid, This, IdType > Base;

    PolyhedralGridIdSet (const Grid& grid)
        : grid_( grid ),
          globalCellPtr_( grid_.globalCellPtr() )
    {
      codimOffset_[ 0 ] = 0;
      for( int i=1; i<=dim; ++i )
      {
        codimOffset_[ i ] = codimOffset_[ i-1 ] + grid.size( i-1 );
      }
    }

    //! id meethod for entity and specific codim
    template< int codim >
    IdType id ( const typename Traits::template Codim< codim >::Entity &entity ) const
    {
      const int index = entity.seed().index();
      // in case
      if (codim == 0 && globalCellPtr_ )
        return IdType( globalCellPtr_[ index ] );
      else
      {
        return codimOffset_[ codim ] + index;
      }
    }

    //! id method of all entities
    template< class Entity >
    IdType id ( const Entity &entity ) const
    {
      return id< Entity::codimension >( entity );
    }

    //! id method of all entities
    template< class IntersectionImpl >
    IdType id ( const Dune::Intersection< const Grid, IntersectionImpl >& intersection ) const
    {
      return Grid::getRealImplementation( intersection ).id();
    }

    //! subId method for entities
    template< class Entity >
    IdType subId ( const Entity &entity, int i, unsigned int codim ) const
    {
      if( codim == 0 )
        return id( entity );
      else if ( codim == 1 )
        return id( entity.template subEntity< 1 >( i ) );
      else if ( codim == dim )
      {
        return id( entity.template subEntity< dim >( i ) );
      }
      else
      {
        DUNE_THROW(NotImplemented,"codimension not available");
        return IdType( -1 );
      }
    }

  protected:
    const Grid& grid_;
    const int* globalCellPtr_;
    IdType codimOffset_[ dim+1 ];
  };

} // namespace Dune

#endif // #ifndef DUNE_POLYHEDRALGRID_IDSET_HH
