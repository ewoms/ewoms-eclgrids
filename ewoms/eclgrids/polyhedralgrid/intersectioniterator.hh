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
#ifndef DUNE_POLYHEDRALGRID_INTERSECTIONITERATOR_HH
#define DUNE_POLYHEDRALGRID_INTERSECTIONITERATOR_HH

#include <ewoms/eclgrids/polyhedralgrid/intersection.hh>

namespace Dune
{

  // PolyhedralGridIntersectionIterator
  // --------------------------

  template< class Grid >
  class PolyhedralGridIntersectionIterator
  {
  protected:
    typedef PolyhedralGridIntersectionIterator< Grid > This;

    typedef typename Grid::Traits Traits;
    typedef typename Traits::template Codim<0>::Entity Element;
    static const bool isLeafIntersection = true;

  public:
    typedef typename Traits::template Codim<0>::EntitySeed EntitySeed;

    typedef typename std::conditional< isLeafIntersection,
                                  typename Traits :: LeafIntersection,
                                  typename Traits :: LevelIntersection > :: type  Intersection ;
#if DUNE_VERSION_NEWER(DUNE_GRID, 2,6)
    typedef typename Intersection :: Implementation IntersectionImpl ;
#else
    typedef typename std::conditional< isLeafIntersection,
                                  typename Traits :: LeafIntersectionImpl,
                                  typename Traits :: LevelIntersectionImpl > :: type  IntersectionImpl ;
#endif

    typedef typename Traits :: ExtraData ExtraData;

    typedef typename Grid::template Codim< 0 >::EntityPointer EntityPointer;

    PolyhedralGridIntersectionIterator ( ExtraData data, const EntitySeed& seed, bool isBegin )
      : intersection_( IntersectionImpl( data, seed, isBegin?0:data->subEntities(seed, 1) ) )
    {}

    PolyhedralGridIntersectionIterator ( const This& other )
      : intersection_( IntersectionImpl( other.intersectionImpl()) )
    {}

    PolyhedralGridIntersectionIterator& operator=( const This& other )
    {
      intersectionImpl() = other.intersectionImpl();
      return *this;
    }

    bool equals ( const This &other ) const
    {
      return intersectionImpl().equals( other.intersectionImpl() );
    }

    void increment ()
    {
      ++(intersectionImpl()).intersectionIdx_;
    }

    const Intersection &dereference () const
    {
      return intersection_;
    }

    ExtraData data() const { return intersectionImpl().data(); }

  protected:
    IntersectionImpl &intersectionImpl () const
    {
#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 7)
      return intersection_.impl();
#else
      return Grid::getRealImplementation( intersection_ );
#endif
    }

    mutable Intersection intersection_;
  };

} // namespace Dune

#endif // #ifndef DUNE_POLYHEDRALGRID_INTERSECTIONITERATOR_HH
