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
#ifndef DUNE_POLYHEDRALGRID_ENTITYPOINTER_HH
#define DUNE_POLYHEDRALGRID_ENTITYPOINTER_HH

//- dune-grid includes
#include <dune/grid/common/grid.hh>

//- dune-metagrid includes
#include <ewoms/eclgrids/polyhedralgrid/declaration.hh>

namespace Dune
{
  // PolyhedralGridEntityPointer
  // -------------------

  template< int codim, class Grid >
  class PolyhedralGridEntityPointer
  {
    typedef PolyhedralGridEntityPointer< codim, Grid > This;

  protected:
    typedef typename Grid::Traits Traits;

  public:
    /** \brief grid dimension */
    static const int dimension = Grid::dimension;
    /** \brief world dimension */
    static const int codimension = codim;

    /** \brief type of entity */
    typedef typename Traits::template Codim< codimension >::Entity Entity;

  protected:
    typedef typename Traits::ExtraData ExtraData;

    typedef typename Traits::template Codim< codimension > :: EntityImpl EntityImpl;

  public:
    PolyhedralGridEntityPointer ( ExtraData data )
    : entity_( EntityImpl( data ) )
    {}

    explicit PolyhedralGridEntityPointer ( const EntityImpl &entity )
    : entity_( EntityImpl( entity ) )
    {}

    PolyhedralGridEntityPointer ( const This &other )
    : entity_( EntityImpl( other.entityImpl() ) )
    {}

    const This &operator= ( const This &other )
    {
      entityImpl() = other.entityImpl();
      return *this;
    }

    /** \brief check for equality */
    bool equals ( const This &other ) const
    {
      return entityImpl().equals( other.entityImpl() );
    }

    /** \brief dereference entity */
    Entity& dereference () const
    {
      return entity_;
    }

    operator const Entity& () const { return entity_; }
    operator       Entity& ()       { return entity_; }

    /** \brief obtain level */
    int level () const { return entity_.level(); }

  protected:
    EntityImpl &entityImpl () const
    {
#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 7)
      return entity_.impl();
#else
      return Grid::getRealImplementation( entity_ );
#endif
    }

    ExtraData data () const { return entityImpl().data(); }

  protected:
    mutable Entity entity_;
  };

} // namespace Dune

#endif // #ifndef DUNE_POLYHEDRALGRID_ENTITYPOINTER_HH
