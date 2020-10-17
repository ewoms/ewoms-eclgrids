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
#ifndef DUNE_POLYHEDRALGRID_ITERATOR_HH
#define DUNE_POLYHEDRALGRID_ITERATOR_HH

#include <dune/geometry/referenceelements.hh>

#include <dune/grid/common/entityiterator.hh>

#include <ewoms/eclgrids/polyhedralgrid/entitypointer.hh>

namespace Dune
{

  // PolyhedralGridIterator
  // --------------

  template< int codim, class Grid, PartitionIteratorType pitype >
  class PolyhedralGridIterator
  : public PolyhedralGridEntityPointer< codim, Grid >
  {
    typedef PolyhedralGridIterator< codim, Grid, pitype > This;
    typedef PolyhedralGridEntityPointer< codim, Grid > Base;

  protected:
    typedef typename Base::ExtraData ExtraData;
    using Base :: entityImpl;

  public:
    typedef typename Grid::Traits::template Codim<codim>::EntitySeed EntitySeed;
    typedef typename Grid::Traits::template Codim<codim>::EntityImpl EntityImpl;

    PolyhedralGridIterator ( ExtraData data, const bool beginIterator )
    : Base( data )
    {
      if( beginIterator )
        entityImpl() = EntityImpl( data, EntitySeed( 0 ) );
    }

    PolyhedralGridIterator ( const This& other )
    : Base( other )
    {}

    /** \brief increment */
    void increment ()
    {
      int index = entityImpl().seed().index();
      ++index;

      if( index >= entityImpl().data()->size( codim ) )
        entityImpl() = EntityImpl( entityImpl().data() );
      else
        entityImpl() = EntityImpl( entityImpl().data(), EntitySeed( index ) );
    }
  };

} // namespace Dune

#endif // #ifndef DUNE_POLYHEDRALGRID_ITERATOR_HH
