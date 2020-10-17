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
#include "config.h"

#define NVERBOSE  // Suppress own messages when throw()ing

#define BOOST_TEST_MODULE CartGridTest
#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>
#if BOOST_VERSION / 100000 == 1 && BOOST_VERSION / 100 % 1000 < 71
#include <boost/test/floating_point_comparison.hpp>
#else
#include <boost/test/tools/floating_point_comparison.hpp>
#endif

/* --- our own headers --- */
#include <ewoms/eclgrids/cart_grid.h>
#include <ewoms/eclgrids/unstructuredgrid.h>
#include <cstdio>

BOOST_AUTO_TEST_SUITE ()

BOOST_AUTO_TEST_CASE (facenumbers)
{
    int faces[] = { 0, 6, 1, 8,
                    1, 7, 2, 9,
                    3, 8, 4, 10,
                    4, 9, 5, 11 };
    struct UnstructuredGrid *g = create_grid_cart2d(2, 2, 1., 1.);
    int i;
    int k;
    for (i = 0; i < g->number_of_cells; ++i) {
        for (k = g->cell_facepos[i]; k < g->cell_facepos[i + 1]; ++k) {
            BOOST_REQUIRE_EQUAL (g->cell_faces[k], faces[k]);
        }
    }
    destroy_grid(g);
}

BOOST_AUTO_TEST_SUITE_END()

