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
#include <dune/common/version.hh>
#include<dune/geometry/referenceelements.hh>
#include "transportproblem2.hh"

//! initialize the vector of unknowns with initial value
template<class G, class M, class V>
void initialize(const G& grid, const M& mapper, V& c)
{
    // first we extract the dimensions of the grid
    const int dim = G::dimension;
    const int dimworld = G::dimensionworld;

    // type used for coordinates in the grid
    typedef typename G::ctype ct;

    // type of grid view on leaf part
    typedef typename G::LeafGridView GridView;

    // leaf iterator type
    typedef typename GridView::template Codim<0>::Iterator LeafIterator;

    // get grid view on leaf part
    GridView gridView = grid.leafGridView();

    // iterate through leaf grid an evaluate c0 at cell center
    LeafIterator endit = gridView.template end<0>();
    for (LeafIterator it = gridView.template begin<0>(); it!=endit; ++it) {
        // get geometry type
        Dune::GeometryType gt = it->type();

        // get cell center in reference element
        const Dune::FieldVector<ct,dim>&
        local = Dune :: ReferenceElements<ct,dim >::general(gt).position(0,0);

        // get global coordinate of cell center
        Dune::FieldVector<ct,dimworld> global =
            it->geometry().global(local);
        // Dune::FieldVector<ct, dimworld> global = it->geometry().position();

#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 4)
        // initialize cell concentration
        c[mapper.index(*it)] = c0(global);
#else
        // initialize cell concentration
        c[mapper.map(*it)] = c0(global);
#endif
    }
}
