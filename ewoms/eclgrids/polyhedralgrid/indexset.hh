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
#ifndef DUNE_POLYHEDRALGRID_INDEXSET_HH
#define DUNE_POLYHEDRALGRID_INDEXSET_HH

#include <vector>

#include <dune/common/typetraits.hh>

#include <dune/grid/common/gridenums.hh>
#include <dune/grid/common/indexidset.hh>

#include <ewoms/eclgrids/polyhedralgrid/declaration.hh>

namespace Dune
{

  // PolyhedralGridIndexSet
  // --------------

  template< int dim, int dimworld, typename coord_t >
  class PolyhedralGridIndexSet
      : public IndexSet< PolyhedralGrid< dim, dimworld, coord_t >, PolyhedralGridIndexSet< dim, dimworld, coord_t >, int >
  {
    typedef PolyhedralGrid<dim, dimworld, coord_t > GridType;

  protected:
    typedef PolyhedralGridIndexSet< dim, dimworld, coord_t > This;
      typedef IndexSet< GridType, This, int > Base;

    typedef typename std::remove_const< GridType >::type::Traits Traits;

  public:
    static const int dimension = Traits::dimension;

    typedef typename Base::IndexType IndexType;

    PolyhedralGridIndexSet ( const GridType& grid )
        : grid_(&grid)
    {
    }

    template< class Entity >
    IndexType index ( const Entity &entity ) const
    {
      return index< Entity::codimension >( entity );
    }

    template< int cd >
    IndexType index ( const typename Traits::template Codim< cd >::Entity &entity ) const
    {
#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 7)
      return entity.impl().index();
#else
      return grid().getRealImplementation(entity).index();
#endif
    }

    template< int cd >
    IndexType subIndex ( const typename Traits::template Codim< cd >::Entity &entity, int i, unsigned int codim ) const
    {
      return subIndex( entity, i, codim );
    }

    template< class Entity >
    IndexType subIndex ( const Entity &entity, int i, unsigned int codim ) const
    {
      if( codim == 0 )
        return index( entity );
      else if ( codim == 1 )
#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 7)
        return index( entity.impl().template subEntity< 1 > ( i ) );
#else
        return index( grid().getRealImplementation( entity ).template subEntity< 1 > ( i ) );
#endif
      else if ( codim == dimension )
      {
#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 7)
        return index( entity.impl().template subEntity< dimension > ( i ) );
#else
        return index( grid().getRealImplementation( entity ).template subEntity< dimension > ( i ) );
#endif
      }
      else
      {
        DUNE_THROW(NotImplemented,"codimension not available");
        return IndexType( -1 );
      }
    }

    IndexType size ( GeometryType type ) const
    {
      return grid().size( type );
    }

    int size ( int codim ) const
    {
      return grid().size( codim );
    }

    template< class Entity >
    bool contains ( const Entity &entity ) const
    {
        return index(entity) >= 0 && index(entity) < size(Entity::codimension);
    }

    const std::vector< GeometryType > &geomTypes ( int codim ) const
    {
        return grid().geomTypes(codim);
    }

    const std::vector< GeometryType >& types(int codim) const
    {
        return grid().geomTypes(codim);
    }

    const GridType& grid() const { assert( grid_ ); return *grid_; }

  protected:
    const GridType *grid_;
  };

} // namespace Dune

#endif // #ifndef DUNE_POLYHEDRALGRID_INDEXSET_HH
