// -*- mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=2 sw=2 sts=2:
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
