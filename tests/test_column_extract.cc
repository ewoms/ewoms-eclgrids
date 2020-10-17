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

#define BOOST_TEST_MODULE ColumnExtractTest
#include <boost/test/unit_test.hpp>
#include <ewoms/eclgrids/columnextract.hh>
#include <ewoms/eclgrids/gridmanager.hh>

#include <ewoms/eclgrids/utility/parserincludes.hh>

#include <cstddef>
#include <iostream>

BOOST_AUTO_TEST_CASE(SingleColumnTest)
{
    using namespace Ewoms;

    const int size_x = 1, size_y = 1, size_z = 10;
    GridManager manager(size_x, size_y, size_z);

    std::vector<std::vector<int> > columns;
    extractColumn(*manager.c_grid(), columns);

    std::vector<int> correct_answer;
    for (int i = 0; i < 10; ++i) {
        correct_answer.push_back(i);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(correct_answer.begin(), correct_answer.end(),
                                  columns[0].begin(), columns[0].end());
}

BOOST_AUTO_TEST_CASE(FourByFourColumnTest)
{
    const int size_x = 4, size_y = 4, size_z = 10;
    using namespace Ewoms;
    GridManager manager(size_x, size_y, size_z);

    std::vector<std::vector<int> > columns;
    extractColumn(*manager.c_grid(), columns);

    std::vector<std::vector<int> > correct_answer;
    correct_answer.resize(size_x * size_y);
    for(int i = 0; i < size_x * size_y; i++) {
        for(int j = 0; j < 10; j++) {
            correct_answer[i].push_back( i + j*size_x*size_y);
        }
    }

    for(int i = 0; i < size_x * size_y; i++) {
        BOOST_CHECK_EQUAL_COLLECTIONS(correct_answer[i].begin(), correct_answer[i].end(),
                                      columns[i].begin(), columns[i].end());
    }
}

BOOST_AUTO_TEST_CASE(DisjointColumn)
{
    std::string grdecl =
"SPECGRID                                        \n"
"3 3 3 1 F                                       \n"
"/                                               \n"
"                                                \n"
"COORD                                           \n"
"0.0 0 0       0.0 0 3.0                         \n"
"1.0 0 0       1.0 0 3.0                         \n"
"2.0 0 0       2.0 0 3.0                         \n"
"3.0 0 0       3.0 0 3.0                         \n"
"                                                \n"
"0.0 1.0 0       0.0 1.0 3.0                     \n"
"1.0 1.0 0       1.0 1.0 3.0                     \n"
"2.0 1.0 0       2.0 1.0 3.0                     \n"
"3.0 1.0 0       3.0 1.0 3.0                     \n"
"                                                \n"
"0.0 2.0 0       0.0 2.0 3.0                     \n"
"1.0 2.0 0       1.0 2.0 3.0                     \n"
"2.0 2.0 0       2.0 2.0 3.0                     \n"
"3.0 2.0 0       3.0 2.0 3.0                     \n"
"                                                \n"
"0.0 3.0 0       0.0 3.0 3.0                     \n"
"1.0 3.0 0       1.0 3.0 3.0                     \n"
"2.0 3.0 0       2.0 3.0 3.0                     \n"
"3.0 3.0 0       3.0 3.0 3.0                     \n"
"/                                               \n"
"                                                \n"
"ZCORN                                           \n"
"36*0.0                                          \n"
"72*1.0                                          \n"
"72*2.0                                          \n"
"36*3.0                                          \n"
"/                                               \n"
"                                                \n"
"ACTNUM                                          \n"
"13*1                                            \n"
"0                                               \n"
"13*1                                            \n"
"/                                               \n"
"\n";

    typedef std::vector< std::vector<int> > VVI;

    const int correct[][3] = { {  0,   9,  17 }  ,  // 0
                               {  1,  10,  18 }  ,  // 1
                               {  2,  11,  19 }  ,  // 2
                               {  3,  12,  20 }  ,  // 3
                               { 21, - 1, - 1 }  ,  // 4
                               {  5,  13,  22 }  ,  // 5
                               {  6,  14,  23 }  ,  // 6
                               {  7,  15,  24 }  ,  // 7
                               {  8,  16,  25 }  ,  // 8
                               {  4, - 1, - 1 } };  // 9

    const std::size_t ncol = (sizeof correct) / (sizeof correct[0]);

    VVI correct_answer(ncol);

    for (std::size_t i = 0; i < ncol; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            correct_answer[i].push_back(correct[i][j]);
        }
    }
    correct_answer[4].resize(1);
    correct_answer[9].resize(1);

#if HAVE_ECL_INPUT
    Ewoms::Parser parser;
    Ewoms::Deck deck = parser.parseString(grdecl);
    Ewoms::EclipseGrid ep = Ewoms::EclipseGrid(deck);
    std::vector<int> actnum;
    for (size_t i = 1; i <= (3 * 3 * 3); i++)
        actnum.push_back(i != 14); // ACTNUM 13*1 0 13* 1
    ep.resetACTNUM(actnum);
    Ewoms::GridManager manager(ep);

    VVI columns;
    Ewoms::extractColumn(*manager.c_grid(), columns);

#if 0
    for (size_t i = 0; i < columns.size(); ++i) {
        for (size_t j = 0; j < columns[i].size(); ++j) {
            std::cout << columns[i][j] << ' ';
        }
        std::cout << '\n';
    }
#endif

    BOOST_CHECK_EQUAL(columns.size(), correct_answer.size());

    for (VVI::iterator
        xb = correct_answer.begin(), xe = correct_answer.end(),
        cb = columns       .begin();
        xb != xe; ++xb, ++cb) {

        BOOST_CHECK_EQUAL_COLLECTIONS((*xb).begin(), (*xb).end(),
                                      (*cb).begin(), (*cb).end());
    }
#endif
}
