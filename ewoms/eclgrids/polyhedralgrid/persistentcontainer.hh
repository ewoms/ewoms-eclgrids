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
#ifndef DUNE_POLYHEDRALGRID_PERSISTENTCONTAINER_HH
#define DUNE_POLYHEDRALGRID_PERSISTENTCONTAINER_HH

#include <dune/common/version.hh>

#include <dune/grid/utility/persistentcontainer.hh>
#include <ewoms/eclgrids/polyhedralgrid/grid.hh>

#include <dune/grid/utility/persistentcontainervector.hh>

namespace Dune
{
  // PersistentContainer for CpGrid
  // -------------------------------
  template< int dim, int dimworld, class Data >
  class PersistentContainer< PolyhedralGrid< dim, dimworld >, Data >
  : public PersistentContainerVector< PolyhedralGrid< dim, dimworld >,
                                      typename PolyhedralGrid< dim, dimworld >::Traits::LeafIndexSet,
                                      std::vector<Data> >
  {
  public:
    typedef PolyhedralGrid< dim, dimworld >  GridType;
    typedef typename std::vector<Data>::allocator_type Allocator;

  private:
    typedef PersistentContainerVector< GridType, typename GridType::Traits::LeafIndexSet, std::vector<Data> > BaseType;

  public:
    //! Constructor filling the container with values using the default constructor
    //! Depending on the implementation this could be achieved without allocating memory
    PersistentContainer ( const GridType &grid, const int codim, const Data& data = Data(), const Allocator &allocator = Allocator() )
    : BaseType( grid.leafIndexSet(), codim, data, allocator )
    {}
  };

} // end namespace Dune

#endif
