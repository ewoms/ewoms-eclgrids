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
#include <config.h>

#define NVERBOSE // to suppress our messages when throwing

#define BOOST_TEST_MODULE GridUtilitiesTest
#include <boost/test/unit_test.hpp>

#include <ewoms/eclgrids/gridutilities.hh>
#include <ewoms/eclgrids/gridmanager.hh>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE(cartesian_2d_cellNeighboursAcrossVertices)
{
    const GridManager gm(2, 2);
    const UnstructuredGrid& grid = *gm.c_grid();
    const SparseTable<int> vnb = cellNeighboursAcrossVertices(grid);

    const int num_elem = 12;
    const int elem[num_elem] = { 1, 2, 3, 0, 2, 3, 0, 1, 3, 0, 1, 2 };
    const int num_rows = 4;
    const int rowsizes[num_rows] = { 3, 3, 3, 3 };
    const SparseTable<int> truth(elem, elem + num_elem, rowsizes, rowsizes + num_rows);
    BOOST_CHECK(vnb == truth);
}

BOOST_AUTO_TEST_CASE(cartesian_3d_cellNeighboursAcrossVertices)
{
    const GridManager gm(3, 2, 2);
    const UnstructuredGrid& grid = *gm.c_grid();
    const SparseTable<int> vnb = cellNeighboursAcrossVertices(grid);

    BOOST_CHECK_EQUAL(int(vnb.size()), grid.number_of_cells);
    BOOST_REQUIRE(!vnb.empty());
    const int n = 7;
    BOOST_CHECK_EQUAL(int(vnb[0].size()), n);
    const int nb[n] = { 1, 3, 4, 6, 7, 9, 10 };
    BOOST_CHECK_EQUAL_COLLECTIONS(vnb[0].begin(), vnb[0].end(), nb, nb + n);
}

BOOST_AUTO_TEST_CASE(cartesian_2d_orderCounterClockwise)
{
    const GridManager gm(2, 2);
    const UnstructuredGrid& grid = *gm.c_grid();
    SparseTable<int> vnb = cellNeighboursAcrossVertices(grid);
    orderCounterClockwise(grid, vnb);

    BOOST_REQUIRE(!vnb.empty());
    const int num_elem = 12;
    const int elem[num_elem] = { 1, 3, 2, 3, 2, 0, 3, 0, 1, 2, 0, 1 };
    const int num_rows = 4;
    const int rowsizes[num_rows] = { 3, 3, 3, 3 };
    const SparseTable<int> truth(elem, elem + num_elem, rowsizes, rowsizes + num_rows);
    BOOST_CHECK(vnb == truth);
    for (int c = 0; c < num_rows; ++c) {
        BOOST_CHECK_EQUAL_COLLECTIONS(vnb[c].begin(), vnb[c].end(), truth[c].begin(), truth[c].end());
    }
}
