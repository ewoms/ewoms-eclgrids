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
#ifndef EWOMS_CORNERPOINT_GRID_HH
#define EWOMS_CORNERPOINT_GRID_HH

/**
 * \file
 * Routines to form a complete UnstructuredGrid from a corner-point
 * specification.
 */

#include <ewoms/eclgrids/unstructuredgrid.h>
#include <ewoms/eclgrids/cpgpreprocess/preprocess.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Construct grid representation from corner-point specification of a
     * particular geological model.
     *
     * Pinched cells will be removed irrespective of any explicit "active" map
     * in the geological model input specification.  Geometric primitives such
     * as cell barycenters (i.e., centroids), volumes and interface areas are
     * computed internally using function compute_geometry().  The caller does
     * not need to compute this information separately.
     *
     * @param[in] in  Corner-point specification.  If "actnum" is NULL, then the
     *                specification is interpreted as if all cells are initially
     *                active.
     *
     * @param[in] tol Absolute tolerance of node-coincidence.
     *
     * @return Fully formed grid data structure that manages the grid defined by
     * the input corner-point specification. Must be destroyed using function
     * destroy_grid().
     */
    struct UnstructuredGrid *
    create_grid_cornerpoint(const struct grdecl *in, double tol);

    /**
     * Compute derived geometric primitives in a grid.
     *
     * This function computes values for each of the following quantities
     * - Quantities pertaining to interfaces (connections, faces)
     *   -# Barycenters (centroids), <CODE>g->dimensions</CODE> scalars per face
     *      stored sequentially in <CODE>g->face_centroids</CODE>.
     *   -# Areas, one scalar per face stored sequentially in
     *      <CODE>g->face_areas</CODE>.
     *   -# Normals, <CODE>g->dimensions</CODE> scalars per face stored
     *      sequentially in <CODE>g->face_normals</CODE>.  The Euclidian norm of
     *      each normal is equal to the corresponding face's area.
     *
     * - Quantities pertaining to cells (volumes)
     *   -# Barycenters (centroids), <CODE>g->dimensions</CODE> scalars per cell
     *      stored sequentially in <CODE>g->cell_centroids</CODE>.
     *   -# Volumes, one scalar per cell stored sequentially in
     *      <CODE>g->cell_volumes</CODE>.
     *
     * These fields must be allocated prior to calling compute_geometry().
     *
     * @param[in,out] g Grid structure.
     */
    void compute_geometry(struct UnstructuredGrid *g);

#ifdef __cplusplus
}
#endif

#endif /* EWOMS_CORNERPOINT_GRID_HH */
