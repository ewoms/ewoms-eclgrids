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
#ifndef DUNE_POLYHEDRALGRID_CAPABILITIES_HH
#define DUNE_POLYHEDRALGRID_CAPABILITIES_HH

//- dune-grid includes
#include <dune/grid/common/capabilities.hh>

//- dune-metagrid includes
#include <ewoms/eclgrids/polyhedralgrid/declaration.hh>

namespace Dune
{

  namespace Capabilities
  {

    // Capabilities from dune-grid
    // ---------------------------

    template< int dim, int dimworld >
    struct hasSingleGeometryType< PolyhedralGrid< dim, dimworld > >
    {
      static const bool v = false;
      static const unsigned int topologyId = ~0u;
    };

    template< int dim, int dimworld >
    struct isCartesian< PolyhedralGrid< dim, dimworld > >
    {
      static const bool v = false;
    };

    template< int dim, int dimworld, int codim >
    struct hasEntity< PolyhedralGrid< dim, dimworld >, codim >
    {
      static const bool v = (codim == 0 || codim == 1 || codim == dim);
    };

#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 5)
    template< int dim, int dimworld, int codim >
    struct hasEntityIterator< PolyhedralGrid< dim, dimworld >, codim >
    {
      static const bool v = (codim == 0 || codim == 1 || codim == dim);
    };
#endif

#if !DUNE_VERSION_NEWER(DUNE_GRID, 2, 5)
    template< int dim, int dimworld >
    struct isParallel< PolyhedralGrid< dim, dimworld > >
    {
        static const bool v = false;
    };
#endif

    template< int dim, int dimworld, int codim >
    struct canCommunicate< PolyhedralGrid< dim, dimworld >, codim >
    {
        static const bool v = false;
    };

    template< int dim, int dimworld >
    struct hasBackupRestoreFacilities< PolyhedralGrid< dim, dimworld > >
    {
      static const bool v = false;
    };

    template< int dim, int dimworld >
    struct isLevelwiseConforming< PolyhedralGrid< dim, dimworld > >
    {
      static const bool v = false;
    };

    template< int dim, int dimworld >
    struct isLeafwiseConforming< PolyhedralGrid< dim, dimworld > >
    {
      static const bool v = false;
    };

    template< int dim, int dimworld >
    struct threadSafe< PolyhedralGrid< dim, dimworld > >
    {
      static const bool v = false;
    };

    template< int dim, int dimworld >
    struct viewThreadSafe< PolyhedralGrid< dim, dimworld > >
    {
      static const bool v = false;
    };

  } // namespace Capabilities

} // namespace Dune

#endif // #ifndef DUNE_POLYHEDRALGRID_CAPABILITIES_HH
