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

#define BOOST_TEST_MODULE SparseTableTest
#include <boost/test/unit_test.hpp>

#include <ewoms/eclgrids/utility/sparsetable.hh>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE(construction_and_queries)
{
    const SparseTable<int> st1;
    BOOST_CHECK(st1.empty());
    BOOST_CHECK_EQUAL(st1.size(), 0);
    BOOST_CHECK_EQUAL(st1.dataSize(), 0);

    // This should be getting us a table like this:
    // ----------------
    // 0
    // <empty row>
    // 1 2
    // 3 4 5 6
    // 7 8 9
    // ----------------
    std::vector<std::vector<int>> expected = { {0}, {}, {1, 2}, {3, 4, 5, 6}, {7, 8, 9} };
    const int num_elem = 10;
    const int elem[num_elem] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    const int num_rows = 5;
    const int rowsizes[num_rows] = { 1, 0, 2, 4, 3 };
    const SparseTable<int> st2(elem, elem + num_elem, rowsizes, rowsizes + num_rows);
    BOOST_CHECK(!st2.empty());
    BOOST_CHECK_EQUAL(st2.size(), num_rows);
    BOOST_CHECK_EQUAL(st2.dataSize(), num_elem);
    BOOST_CHECK_EQUAL(st2[0][0], 0);
    BOOST_CHECK_EQUAL(st2.rowSize(0), 1);
    BOOST_CHECK(st2[1].empty());
    BOOST_CHECK_EQUAL(st2.rowSize(1), 0);
    BOOST_CHECK_EQUAL(st2[3][1], 4);
    BOOST_CHECK_EQUAL(st2[4][2], 9);
    BOOST_CHECK(int(st2[4].size()) == rowsizes[4]);
    const SparseTable<int> st2_again(elem, elem + num_elem, rowsizes, rowsizes + num_rows);
    BOOST_CHECK(st2 == st2_again);
    SparseTable<int> st2_byassign;
    st2_byassign.assign(elem, elem + num_elem, rowsizes, rowsizes + num_rows);
    BOOST_CHECK(st2 == st2_byassign);
    const int last_row_size = rowsizes[num_rows - 1];
    SparseTable<int> st2_append(elem, elem + num_elem - last_row_size, rowsizes, rowsizes + num_rows - 1);
    BOOST_CHECK_EQUAL(st2_append.dataSize(), num_elem - last_row_size);
    st2_append.appendRow(elem + num_elem - last_row_size, elem + num_elem);
    BOOST_CHECK(st2 == st2_append);
    SparseTable<int> st2_append2;
    st2_append2.appendRow(elem, elem + 1);
    st2_append2.appendRow(elem + 1, elem + 1);
    st2_append2.appendRow(elem + 1, elem + 3);
    st2_append2.appendRow(elem + 3, elem + 7);
    st2_append2.appendRow(elem + 7, elem + 10);
    BOOST_CHECK(st2 == st2_append2);
    st2_append2.clear();
    SparseTable<int> st_empty;
    BOOST_CHECK(st2_append2 == st_empty);

    SparseTable<int> st2_allocate;
    st2_allocate.allocate(rowsizes, rowsizes + num_rows);
    BOOST_CHECK_EQUAL(st2_allocate.size(), num_rows);
    BOOST_CHECK_EQUAL(st2_allocate.dataSize(), num_elem);
    int s = 0;
    for (int i = 0; i < num_rows; ++i) {
        SparseTable<int>::mutable_row_type row = st2_allocate[i];
        for (int j = 0; j < rowsizes[i]; ++j, ++s)
            row[j] = elem[s];
    }
    BOOST_CHECK(st2 == st2_allocate);

    // One element too few.
    BOOST_CHECK_THROW(const SparseTable<int> st3(elem, elem + num_elem - 1, rowsizes, rowsizes + num_rows), std::exception);

    // A few elements too many.
    BOOST_CHECK_THROW(const SparseTable<int> st4(elem, elem + num_elem, rowsizes, rowsizes + num_rows - 1), std::exception);

    // Need at least one row.
    BOOST_CHECK_THROW(const SparseTable<int> st5(elem, elem + num_elem, rowsizes, rowsizes), std::exception);

    // Test iteration over rows with a range-for loop.
    int row_index = 0;
    for (const auto& row : st2) {
        BOOST_CHECK_EQUAL(row.size(), expected[row_index].size());
        ++row_index;
    }

    // Tests that only run in debug mode.
#ifndef NDEBUG
    // Do not ask for wrong row numbers.
    BOOST_CHECK_THROW(st1.rowSize(0), std::exception);
    BOOST_CHECK_THROW(st2.rowSize(-1), std::exception);
    BOOST_CHECK_THROW(st2.rowSize(st2.size()), std::exception);
    // No negative row sizes.
    const int err_rs[num_rows] = { 1, 0, -1, 7, 3 };
    BOOST_CHECK_THROW(const SparseTable<int> st6(elem, elem + num_elem, err_rs, err_rs + num_rows), std::exception);
#endif
}
