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

#define BOOST_TEST_MODULE MinpvProcessorTest
#include <boost/test/unit_test.hpp>

#include <ewoms/eclgrids/minpvprocessor.hh>

BOOST_AUTO_TEST_CASE(Pinch)
{
    // Set up a simple example.
    std::vector<double> zcorn = { 0, 0, 0, 0,
                                  2, 2, 2, 2,
                                  2, 2, 2, 2,
                                  2.5, 2.5, 2.5, 2.5,
                                  2.5, 2.5, 2.5, 2.5,
                                  3.5, 3.5, 3.5, 3.5,
                                  3.5, 3.5, 3.5, 3.5,
                                  6, 6, 6, 6 };

    std::vector<double> pv = { 2, 0.5, 1, 2.5};
    std::vector<int> actnum = { 1, 1, 1, 1 };
    std::vector<double> thickness = {2, 0.5, 1, 2.5};
    double z_threshold = 0.4;

    Ewoms::MinpvProcessor mp1(1, 1, 4);
    auto z1 = zcorn;
    std::vector<double> minpvv(4, 0.6);
    bool fill_removed_cells = false;
    bool pinch_no_gap = false;
    std::vector<double> zcornAfter =
        {0, 0, 0, 0,
         2, 2, 2, 2,
         2, 2, 2, 2,
         2, 2, 2, 2,
         2.5, 2.5, 2.5, 2.5,
         3.5, 3.5, 3.5, 3.5,
         3.5, 3.5, 3.5, 3.5,
         6, 6, 6, 6
        };
    auto nnc = mp1.process(thickness, z_threshold, pv, minpvv, actnum, fill_removed_cells, z1.data(), pinch_no_gap);
    BOOST_CHECK_EQUAL_COLLECTIONS(z1.begin(), z1.end(), zcornAfter.begin(), zcornAfter.end());
    BOOST_CHECK_EQUAL(nnc.size(), 1);

    z1= zcorn;
    pinch_no_gap = true;
    nnc = mp1.process(thickness, z_threshold, pv, minpvv, actnum, fill_removed_cells, z1.data(), pinch_no_gap);
    BOOST_CHECK_EQUAL_COLLECTIONS(z1.begin(), z1.end(), zcornAfter.begin(), zcornAfter.end());
    BOOST_CHECK_EQUAL(nnc.size(), 1);

    z_threshold = 0.4;
    pinch_no_gap = true;
    minpvv = std::vector<double>(4, 0.6);
    z1 = zcorn;
    nnc = mp1.process(thickness, z_threshold, pv, minpvv, actnum, fill_removed_cells, z1.data(), pinch_no_gap);

    BOOST_CHECK_EQUAL(nnc.size(), 1);
    BOOST_CHECK_EQUAL(nnc[0], 2);
    BOOST_CHECK_EQUAL_COLLECTIONS(z1.begin(), z1.end(), zcornAfter.begin(), zcornAfter.end());

    z_threshold = 0.6;
    pinch_no_gap = true;
    minpvv = std::vector<double>(4, 0.4);
    z1 = zcorn;
    nnc = mp1.process(thickness, z_threshold, pv, minpvv, actnum, fill_removed_cells, z1.data(), pinch_no_gap);

    BOOST_CHECK_EQUAL(nnc.size(), 0);
    BOOST_CHECK_EQUAL_COLLECTIONS(z1.begin(), z1.end(), zcorn.begin(), zcorn.end());

    z_threshold = 1.1;
    pinch_no_gap = true;
    minpvv = std::vector<double>(4, 1.1);
    z1 = zcorn;
    nnc = mp1.process(thickness, z_threshold, pv, minpvv, actnum, fill_removed_cells, z1.data(), pinch_no_gap);
    zcornAfter =
        {0, 0, 0, 0,
         2, 2, 2, 2,
         2, 2, 2, 2,
         2, 2, 2, 2,
         2.5, 2.5, 2.5, 2.5,
         2.5, 2.5, 2.5, 2.5,
         3.5, 3.5, 3.5, 3.5,
         6, 6, 6, 6
        };
    BOOST_CHECK_EQUAL(nnc.size(), 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(z1.begin(), z1.end(), zcornAfter.begin(), zcornAfter.end());
}

BOOST_AUTO_TEST_CASE(Processing)
{
    std::vector<double> zcorn = { 0, 0, 0, 0,
                                  2, 2, 2, 2,
                                  2, 2, 2, 2,
                                  3, 3, 3, 3,
                                  3, 3, 3, 3,
                                  3, 3, 3, 3,
                                  3, 3, 3, 3,
                                  6, 6, 6, 6 };
    std::vector<double> zcorn2after = { 0, 0, 0, 0,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        6, 6, 6, 6 };
    std::vector<double> zcorn3after = { 0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        6, 6, 6, 6  };
    std::vector<double> zcorn4after = { 0, 0, 0, 0,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        3, 3, 3, 3,
                                        6, 6, 6, 6 };
    std::vector<double> zcorn5after = { 0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        2, 2, 2, 2,
                                        3, 3, 3, 3,
                                        6, 6, 6, 6 };

    std::vector<double> pv = { 2, 1, 0, 3};
    std::vector<int> actnum = { 1, 1, 0, 1 };
    std::vector<int> actnum_empty;
    std::vector<double> thicknes = {2, 1, 0, 3};
    double z_threshold = 0.0;

    Ewoms::MinpvProcessor mp1(1, 1, 4);
    auto z1 = zcorn;
    std::vector<double> minpvv1(4, 0.5);
    bool fill_removed_cells = true;
    mp1.process(thicknes, z_threshold, pv, minpvv1, actnum, fill_removed_cells, z1.data());
    BOOST_CHECK_EQUAL_COLLECTIONS(z1.begin(), z1.end(), zcorn.begin(), zcorn.end());

    // check that it is possible to pass empty actnum
    Ewoms::MinpvProcessor mp1b(1, 1, 4);
    auto z1b = zcorn;
    mp1b.process(thicknes, z_threshold, pv, minpvv1, actnum_empty, fill_removed_cells, z1b.data());
    BOOST_CHECK_EQUAL_COLLECTIONS(z1b.begin(), z1b.end(), zcorn.begin(), zcorn.end());

    Ewoms::MinpvProcessor mp2(1, 1, 4);
    auto z2 = zcorn;
    std::vector<double> minpvv2(4, 1.5);
    mp2.process(thicknes, z_threshold, pv, minpvv2, actnum, fill_removed_cells, z2.data());
    BOOST_CHECK_EQUAL_COLLECTIONS(z2.begin(), z2.end(), zcorn2after.begin(), zcorn2after.end());

    Ewoms::MinpvProcessor mp3(1, 1, 4);
    auto z3 = zcorn;
    std::vector<double> minpvv3(4, 2.5);
    auto nnc3 = mp3.process(thicknes, z_threshold, pv, minpvv3, actnum, fill_removed_cells, z3.data());
    BOOST_CHECK_EQUAL(nnc3.size(), 0);
    BOOST_CHECK_EQUAL_COLLECTIONS(z3.begin(), z3.end(), zcorn3after.begin(), zcorn3after.end());

    Ewoms::MinpvProcessor mp4(1, 1, 4);
    auto z4 = zcorn;
    auto nnc4 = mp4.process(thicknes, z_threshold, pv, minpvv2, actnum, !fill_removed_cells, z4.data());
    BOOST_CHECK_EQUAL(nnc4.size(), 1);
    BOOST_CHECK_EQUAL(nnc4.at(0), 3);
    BOOST_CHECK_EQUAL_COLLECTIONS(z4.begin(), z4.end(), zcorn4after.begin(), zcorn4after.end());

    Ewoms::MinpvProcessor mp5(1, 1, 4);
    auto z5 = zcorn;
    auto nnc5 = mp5.process(thicknes, z_threshold, pv, minpvv3, actnum, !fill_removed_cells, z5.data());
    BOOST_CHECK_EQUAL(nnc5.size(), 0);
    BOOST_CHECK_EQUAL_COLLECTIONS(z5.begin(), z5.end(), zcorn5after.begin(), zcorn5after.end());

    Ewoms::MinpvProcessor mp6(1, 1, 4);
    auto z6 = zcorn;
    std::vector<double> minpvv6 = {1, 2, 2, 1};
    auto nnc6 = mp6.process(thicknes, z_threshold, pv, minpvv6, actnum, !fill_removed_cells, z6.data());
    BOOST_CHECK_EQUAL(nnc6.size(), 1);
    BOOST_CHECK_EQUAL_COLLECTIONS(z6.begin(), z6.end(), zcorn4after.begin(), zcorn4after.end());
}
