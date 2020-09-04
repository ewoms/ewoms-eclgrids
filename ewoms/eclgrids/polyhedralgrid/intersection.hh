// -*- mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=2 sw=2 sts=2:
#ifndef DUNE_POLYHEDRALGRID_INTERSECTION_HH
#define DUNE_POLYHEDRALGRID_INTERSECTION_HH

//- dune-common includes
#include <dune/common/version.hh>

//- local includes
#include <ewoms/eclgrids/polyhedralgrid/declaration.hh>

namespace Dune
{

  // PolyhedralGridIntersection
  // ------------------

  template< class Grid >
  class PolyhedralGridIntersection
  {
    typedef PolyhedralGridIntersection< Grid > This;
  protected:
    typedef typename Grid :: Traits Traits;

    typedef typename Traits :: ExtraData ExtraData ;

  public:
    typedef typename Traits::ctype ctype;
    typedef typename Traits::GlobalCoordinate GlobalCoordinate;

    static const int dimension = Traits::dimension;
    static const int dimensionworld = Traits::dimensionworld;

    typedef typename Traits::template Codim< 0 >::Entity Entity;
    typedef typename Traits::template Codim< 0 >::EntityPointer EntityPointer;
    typedef typename Traits::template Codim< 0 >::EntitySeed    EntitySeed;
    typedef typename Traits::template Codim< 1 >::Geometry Geometry;
    typedef typename Traits::template Codim< 1 >::LocalGeometry LocalGeometry;

  protected:
    typedef typename Traits::template Codim< 0 >::EntityPointerImpl EntityPointerImpl;
    typedef typename Traits::template Codim< 0 >::EntityImpl EntityImpl;
    typedef typename Traits::template Codim< 1 >::GeometryImpl GeometryImpl;
    typedef typename Traits::template Codim< 1 >::LocalGeometryImpl LocalGeometryImpl;

  public:
    explicit PolyhedralGridIntersection ( ExtraData data )
    : data_( data ),
      seed_(),
      intersectionIdx_( -1 )
    {}

    PolyhedralGridIntersection ( ExtraData data, const EntitySeed& seed, const int intersectionIdx )
    : data_( data ),
      seed_( seed ),
      intersectionIdx_( intersectionIdx )
    {}

    PolyhedralGridIntersection ( const This& other )
    : data_( other.data_ ),
      seed_( other.seed_ ),
      intersectionIdx_( other.intersectionIdx_ )
    {}

    Entity inside () const
    {
        return Entity( EntityImpl( data(), seed_ ) );
    }

    Entity outside () const
    {
      return Entity( EntityImpl(data(),
                                data()->neighbor(seed_, intersectionIdx_)) );
    }

    PolyhedralGridIntersection& operator=(const PolyhedralGridIntersection& other)
    {
      data_ = other.data_;
      seed_ = other.seed_;
      intersectionIdx_ = other.intersectionIdx_;
      return *this;
    }

    bool operator == ( const This& other ) const
    {
      return (seed_ == other.seed_) &&
             (intersectionIdx_ == other.intersectionIdx_);
    }

    bool boundary () const { return !neighbor(); }

    bool conforming () const { return false; }

    bool neighbor () const { return data()->neighbor(seed_, intersectionIdx_).isValid(); }

    int boundaryId () const { return 1; }

    size_t boundarySegmentIndex () const
    {
        return data()->boundarySegmentIndex( seed_, intersectionIdx_);
    }

    LocalGeometry geometryInInside () const
    {
      return LocalGeometry( LocalGeometryImpl( data() ) );
    }

    LocalGeometry geometryInOutside () const
    {
      return LocalGeometry( LocalGeometryImpl( data() ) );
    }

    Geometry geometry () const
    {
      return Geometry( GeometryImpl(data(), data()->template subEntitySeed<1>(seed_, intersectionIdx_)));
    }

    GeometryType type () const
    {
#if DUNE_VERSION_NEWER(DUNE_GEOMETRY, 2, 6)
        return Dune::GeometryTypes::cube(dimension);
#else
        return GeometryType(GeometryType::cube, dimension);
#endif
    }

    int indexInInside () const
    {
        return data()->indexInInside(seed_, intersectionIdx_);
    }

    int indexInOutside () const
    {
        return data()->indexInOutside(seed_, intersectionIdx_);
    }

    GlobalCoordinate
    integrationOuterNormal ( const FieldVector< ctype, dimension-1 > &local ) const
    {
      return outerNormal( local );
    }

    GlobalCoordinate
    outerNormal ( const FieldVector< ctype, dimension-1 > & ) const
    { return outerNormal(); }

    GlobalCoordinate outerNormal () const
    { return data()->outerNormal(seed_, intersectionIdx_); }

    GlobalCoordinate
    unitOuterNormal ( const FieldVector< ctype, dimension-1 > & ) const
    {
      return centerUnitOuterNormal();
    }

    GlobalCoordinate
    centerUnitOuterNormal () const
    { return data()->unitOuterNormal(seed_, intersectionIdx_); }

    ExtraData data() const { return data_; }

    bool equals(const This& other) const
    {
      return seed_.equals(other.seed_) && intersectionIdx_ == other.intersectionIdx_;
    }

    // intersection id (here index of the face in the grid)
    int id() const
    {
      // return face number of current intersection
      return data()->template subEntitySeed<1>( seed_, intersectionIdx_).index();
    }

  protected:
    ExtraData  data_;
    EntitySeed seed_;
  public:
    int intersectionIdx_; // the element-local index
  };

} // namespace Dune

#endif // #ifndef DUNE_POLYHEDRALGRID_INTERSECTION_HH
