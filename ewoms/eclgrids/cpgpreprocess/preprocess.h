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
#ifndef EWOMS_PREPROCESS_HEADER
#define EWOMS_PREPROCESS_HEADER

/**
 * \file
 * Low-level corner-point processing routines and supporting data structures.
 *
 * User code should typically employ higher-level routines such as
 * create_grid_cornerpoint() in order to construct fully formed UnstructuredGrid
 * data structures from a corner-point specification. Incidentally, the routines
 * provided by this module are used to implement function
 * create_grid_cornerpoint().
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Raw corner-point specification of a particular geological model.
     */
    struct grdecl {
        int           dims[3]; /**< Cartesian box dimensions. */
        const double *coord;   /**< Pillar end-points. */
        const double *zcorn;   /**< Corner-point depths. */
        const int    *actnum;  /**< Explicit "active" map.  May be NULL.*/
        const double *mapaxes; /**< 6 Element rotation vector - can be NULL. */
    };

    /**
     * Connection taxonomy.
     */
    enum face_tag {
        I_FACE,        /**< Connection topologically normal to J-K plane. */
        J_FACE,        /**< Connection topologically normal to I-K plane. */
        K_FACE,        /**< Connection topologically normal to I-J plane. */
        NNC_FACE       /**< Arbitrary non-neighbouring connection. */
    };

    /**
     * Result structure representing minimal derived topology and geometry of
     * a geological model in corner-point format.
     */
    struct processed_grid {
        int m; /**< Upper bound on "number_of_faces".  For internal use in
                    function process_grid()'s memory management. */
        int n; /**< Upper bound on "number_of_nodes".  For internal use in
                    function process_grid()'s memory management. */

        int    dimensions[3];     /**< Cartesian box dimensions. */

        int    number_of_faces;   /**< Total number of unique grid faces
                                       (i.e., connections). */
        int    *face_nodes;       /**< Node (vertex) numbers of each face,
                                       stored sequentially. */
        int    *face_ptr;         /**< Start position for each face's
                                       `face_nodes'. */
        int    *face_neighbors;   /**< Global cell numbers.  Two elements per
                                       face, stored sequentially. */
        enum face_tag *face_tag;  /**< Classification of grid's individual
                                       connections (faces). */

        int    number_of_nodes;   /**< Number of unique grid vertices. */
        int    number_of_nodes_on_pillars; /**< Total number of unique cell
                                                vertices that lie on pillars. */
        double *node_coordinates; /**< Vertex coordinates.  Three doubles
                                       (\f$x\f$, \f$y\f$, \f$z\f$) per vertex,
                                       stored sequentially. */

        int    number_of_cells;   /**< Number of active grid cells. */
        int    *local_cell_index; /**< Deceptively named local-to-global cell
                                       index mapping. */
    };

    /**
     * Construct a prototypical grid representation from a corner-point
     * specification.
     *
     * Pinched cells will be removed irrespective of any explicit "active" map
     * in the geological model input specification. On input, the result
     * structure "out" must point to a valid management structure. In other
     * words, the result structure must point to a region of memory that is
     * typically backed by automatic or allocated (dynamic) storage duration.
     *
     * @param[in]     g   Corner-point specification. If "actnum" is NULL, then
     *                    the specification is interpreted as if all cells are
     *                    initially active.
     * @param[in]     tol Absolute tolerance of node-coincidence.
     * @param[in,out] out Minimal grid representation featuring face-to-cell
     *                    neighbourship definition, vertex geometry, face's
     *                    constituent vertices, and local-to-global cell
     *                    mapping.
     */
    void process_grdecl(const struct grdecl   *g  ,
                        double                 tol,
                        struct processed_grid *out);

    /**
     * Release memory resources acquired in previous grid processing using
     * function process_grdecl().
     *
     * Note: This function releases the resources associated to the individual
     * fields of the processed_grid, but does not free() the structure itself.
     *
     * @param[in,out] g Prototypical grid representation obtained in an earlier
     *                  call to function process_grdecl().
     */
    void free_processed_grid(struct processed_grid *g);
#ifdef __cplusplus
}
#endif

#endif /* EWOMS_PREPROCESS_HEADER */

/* Local Variables:    */
/* c-basic-offset:4    */
/* End:                */
