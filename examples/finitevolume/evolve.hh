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

template<class G, class M, class V>
void evolve(const G& grid, const M& mapper, V& c, double t, double& dt)
{
    // first we extract the dimensions of the grid
    const int dim = G::dimension;
    const int dimworld = G::dimensionworld;

    // type used for coordinates in the grid
    typedef typename G::ctype ct;

    // type of grid view on leaf part
    typedef typename G::LeafGridView GridView;

    // element iterator type
    typedef typename GridView::template Codim<0>::Iterator LeafIterator;

    // intersection iterator type
    typedef typename GridView::IntersectionIterator IntersectionIterator;

    // get grid view on leaf part
    GridView gridView = grid.leafGridView();

    // allocate a temporary vector for the update
    V update(c.size());                                  /*@\label{evh:update}@*/
    for (typename V::size_type i=0; i<c.size(); i++) update[i] = 0;

    // initialize dt very large
    dt = 1E100;

    // compute update vector and optimum dt in one grid traversal
    LeafIterator endit = gridView.template end<0>();     /*@\label{evh:loop0}@*/
    for (LeafIterator it = gridView.template begin<0>(); it!=endit; ++it) {
        // cell geometry type
        Dune::GeometryType gt = it->type();

        // cell center in reference element
        const Dune::FieldVector<ct,dim>&
        local = Dune::ReferenceElements<ct,dim>::general(gt).position(0,0);

        // cell center in global coordinates
        /*        Dune::FieldVector<ct,dimworld>
                  global = it->geometry().center(); */

        // cell volume, assume linear map here
        double volume = it->geometry().integrationElement(local)
            *Dune::ReferenceElements<ct,dim>::general(gt).volume();
        // double volume = it->geometry().volume();

#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 4)
        // cell index
        int indexi = mapper.index(*it);
#else
        // cell index
        int indexi = mapper.map(*it);
#endif

        // variable to compute sum of positive factors
        double sumfactor = 0.0;
        // run through all intersections with neighbors and boundary
        IntersectionIterator isend = gridView.iend(*it); /*@\label{evh:flux0}@*/
        for (IntersectionIterator is = gridView.ibegin(*it); is!=isend; ++is) {

            // get geometry type of face
            // Dune::GeometryType gtf = is->intersectionSelfLocal().type();
            Dune::GeometryType gtf = is->type();

            // center in face's reference element
            const Dune::FieldVector<ct,dim-1>&
            facelocal = Dune::ReferenceElements<ct,dim-1>::general(gtf).position(0,0);

            // get normal vector scaled with volume
            Dune::FieldVector<ct,dimworld> integrationOuterNormal
            = is->integrationOuterNormal(facelocal);
            integrationOuterNormal
            *= Dune::ReferenceElements<ct,dim-1>::general(gtf).volume();

            // center of face in global coordinates
            Dune::FieldVector<ct,dimworld>
                faceglobal = is->geometry().center();

            // evaluate velocity at face center
            Dune::FieldVector<double,dim> velocity = u(faceglobal,t);

            // compute factor occuring in flux formula
            double factor = velocity*integrationOuterNormal/volume;

            // for time step calculation
            if (factor>=0) sumfactor += factor;

            // handle interior face
            if (is->neighbor()) // "correct" version /*@\label{evh:neighbor}@*/
            {
                // access neighbor
                const auto& outside = is->outside();
#if DUNE_VERSION_NEWER(DUNE_GRID, 2, 4)
                int indexj = mapper.index(outside);
#else
                int indexj = mapper.map(*outside);
#endif

                // compute flux from one side only
                // this should become easier with the new IntersectionIterator functionality!
                if (it->level()>outside->level() ||
                        (it->level()==outside->level() && indexi<indexj))
                {
                    // compute factor in neighbor
                    Dune::GeometryType nbgt = outside->type();
                    const Dune::FieldVector<ct,dim>&
                    nblocal = Dune::ReferenceElements<ct,dim>::general(nbgt).position(0,0);
                    double nbvolume = outside->geometry().integrationElement(nblocal)
                                      *Dune::ReferenceElements<ct,dim>::general(nbgt).volume();

                    double nbfactor = velocity*integrationOuterNormal/nbvolume;

                    if (factor<0) // inflow
                    {
                        update[indexi] -= c[indexj]*factor;
                        update[indexj] += c[indexj]*nbfactor;
                    } else // outflow
                    {
                        update[indexi] -= c[indexi]*factor;
                        update[indexj] += c[indexi]*nbfactor;
                    }
                }
            }

            // handle boundary face
            if (is->boundary())                   /*@\label{evh:bndry}@*/
            {
                if (factor<0) // inflow, apply boundary condition
                    update[indexi] -= b(faceglobal,t)*factor;
                else // outflow
                    update[indexi] -= c[indexi]*factor;
            }
        } // end all intersections             /*@\label{evh:flux1}@*/
        // compute dt restriction
        dt = std::min(dt,1.0/sumfactor);             /*@\label{evh:dt}@*/

    } // end grid traversal                        /*@\label{evh:loop1}@*/

    // scale dt with safety factor
    dt *= 0.99;                                          /*@\label{evh:.99}@*/

    // update the concentration vector
    for (unsigned int i=0; i<c.size(); ++i)
        c[i] += dt*update[i];                          /*@\label{evh:updc}@*/

    return;
}
