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
#ifndef DUNE_POLYHEDRALGRID_ENTITYSEED_HH
#define DUNE_POLYHEDRALGRID_ENTITYSEED_HH

#include <dune/common/version.hh>
#include <dune/common/typetraits.hh>

#include <dune/grid/common/entityseed.hh>

namespace Dune
{

  template< int codim, class Grd >
  class PolyhedralGridEntitySeed
  {
    typedef typename std::remove_const< Grd >::type::Traits Traits;

  public:
    static const int codimension = codim;
    static const int dimension = Traits::dimension;
    static const int mydimension = dimension - codimension;
    static const int dimensionworld = Traits::dimensionworld;

    typedef typename Traits::Grid Grid;
    typedef typename Traits::template Codim< codim >::Entity Entity;
    typedef typename Traits :: Index Index ;

    static const Index defaultIndex = -1;

    explicit PolyhedralGridEntitySeed ( const Index& index )
      : index_( index )
    {}

    PolyhedralGridEntitySeed ()
      : index_( defaultIndex )
    {}

    int index () const { return index_ ; }

    // check that index is valid, which means >= 0
    // boundary faces can be arbitrary number < 0
    bool isValid() const { return index_ > defaultIndex; }

    bool equals(const PolyhedralGridEntitySeed& other) const
    { return index_ == other.index_; }

  protected:
    Index index_;
  };

} // namespace Dune

#endif // #ifndef DUNE_POLYHEDRALGRID_ENTITYSEED_HH
