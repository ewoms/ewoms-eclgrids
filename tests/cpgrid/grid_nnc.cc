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

#define NVERBOSE

#define BOOST_TEST_MODULE CpGridWithNNC

#include <boost/test/unit_test.hpp>

#include <dune/common/version.hh>
#include <dune/grid/common/mcmgmapper.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclgrids/cpgrid.hh>
#include <vector>
#include <utility>

namespace std
{
    ostream& operator<<(ostream& os, const pair<int, int>& x)
    {
        os << "{ " << x.first << ", " << x.second << " }";
        return os;
    }
}

#if DUNE_VERSION_NEWER(DUNE_GRID, 2,6)
using ElementMapper = Dune::MultipleCodimMultipleGeomTypeMapper<Dune::CpGrid::LeafGridView>;
#else
using ElementMapper = Dune::MultipleCodimMultipleGeomTypeMapper<Dune::CpGrid::LeafGridView, Dune::MCMGElementLayout >;
#endif

struct Fixture
{
    Fixture()
    {
        int m_argc = boost::unit_test::framework::master_test_suite().argc;
        char** m_argv = boost::unit_test::framework::master_test_suite().argv;
        Dune::MPIHelper::instance(m_argc, m_argv);
        Ewoms::OpmLog::setupSimpleDefaultLogging();
    }

    Ewoms::Parser parser;

    void testCase(const std::string& filename,
                  const Ewoms::NNC& nnc,
                  const int ex_elemcount,
                  const int ex_intercount,
                  const int ex_bdycount,
                  const std::vector<std::pair<int, int>>& ex_nb,
                  const bool use_deck_porv = false)
    {
        Ewoms::EclipseState es(parser.parseFile(filename));
        std::vector<double> porv;

        if (use_deck_porv) {
            porv = es.fieldProps().porv(true);
        }

        Dune::CpGrid grid;
        grid.processEclipseFormat(&es.getInputGrid(), false, false, false, porv, nnc);
        const auto& gv = grid.leafGridView();
#if DUNE_VERSION_NEWER(DUNE_GRID, 2,6)
        ElementMapper elmap(gv, Dune::mcmgElementLayout());
#else
        ElementMapper elmap(gv);
#endif
        int elemcount = 0;
        int intercount = 0;
        int bdycount = 0;
        std::vector<std::pair<int, int>> nb;
        for (const auto& elem : elements(gv)) {
            ++elemcount;
            for (const auto& inter : intersections(gv, elem)) {
                ++intercount;
                if (inter.boundary()) {
                    ++bdycount;
                } else {
                    // Internal connection, store in vector of pairs, if c1 < c2.
                    const int c1 = elmap.index(elem);
                    const int c2 = elmap.index(inter.outside());
                    if (c1 < c2) {
                        nb.push_back(std::make_pair(c1, c2));
                    }
                }
            }
        }
        BOOST_CHECK_EQUAL(elemcount, ex_elemcount);
        BOOST_CHECK_EQUAL(intercount, ex_intercount);
        BOOST_CHECK_EQUAL(bdycount, ex_bdycount);
        std::sort(nb.begin(), nb.end());
        //BOOST_TEST(nb == ex_nb, boost::test_tools::per_element());
	BOOST_CHECK_EQUAL_COLLECTIONS(nb.begin(), nb.end(),
                                      ex_nb.begin(), ex_nb.end());
    }
};

BOOST_AUTO_TEST_SUITE(ConstructingWithNNC)

BOOST_FIXTURE_TEST_CASE(NoNNC, Fixture)
{
    Ewoms::NNC nnc;
    testCase("FIVE.DATA", nnc, 5, 30, 22, { {0,1}, {1,2}, {2,3}, {3,4} });
}

BOOST_FIXTURE_TEST_CASE(NNCAtExistingFace, Fixture)
{
    Ewoms::NNC nnc;
    // Should not change grid since cells are already neighbours.
    // Repeated NNCs do not change the grid further.
    nnc.addNNC(2, 3, 1.0);
    nnc.addNNC(3, 2, 1.0);
    nnc.addNNC(2, 3, 1.0);
    testCase("FIVE.DATA", nnc, 5, 30, 22, { {0,1}, {1,2}, {2,3}, {3,4} });
}

BOOST_FIXTURE_TEST_CASE(NNCAtNewFace, Fixture)
{
    Ewoms::NNC nnc;
    nnc.addNNC(2, 4, 1.0);
    testCase("FIVE.DATA", nnc, 5, 30 + 2, 22, { {0,1}, {1,2}, {2,3}, {2,4}, {3,4} });
}

BOOST_FIXTURE_TEST_CASE(NNCAtSeveralFaces, Fixture)
{
    Ewoms::NNC nnc;
    nnc.addNNC(2, 4, 1.0);   // new connection
    nnc.addNNC(3, 4, 1.0);
    nnc.addNNC(2, 1, 1.0);
    nnc.addNNC(1, 2, 1.0);
    nnc.addNNC(1, 4, 1.0);   // new connection
    nnc.addNNC(999, 2, 1.0); // invalid
    nnc.addNNC(-1, 2, 1.0);  // invalid
    testCase("FIVE.DATA", nnc, 5, 30 + 4, 22, { {0,1}, {1,2}, {1,4}, {2,3}, {2,4}, {3,4} });
}

BOOST_FIXTURE_TEST_CASE(ActnumNoNNC, Fixture)
{
    Ewoms::NNC nnc;
    testCase("FIVE_ACTNUM.DATA", nnc, 4, 24, 20, { {0,1}, {2,3} });
}

BOOST_FIXTURE_TEST_CASE(ActnumNNCAtExistingFace, Fixture)
{
    Ewoms::NNC nnc;
    // Should not change grid since cells are already neighbours.
    // Note that cells 3 and 4 in the NNC spec are logical cartesian (global) faces,
    // since global cell 2 is inactive, this becomes cells 2 and 3 in active numbering.
    nnc.addNNC(3, 4, 1.0);
    testCase("FIVE_ACTNUM.DATA", nnc, 4, 24, 20, { {0,1}, {2,3} });
}

BOOST_FIXTURE_TEST_CASE(ActnumNNCAtNewFace, Fixture)
{
    Ewoms::NNC nnc;
    nnc.addNNC(2, 4, 1.0); // inactive cell 2, no new connection
    nnc.addNNC(1, 4, 1.0); // new connection
    testCase("FIVE_ACTNUM.DATA", nnc, 4, 24 + 2, 20, { {0,1}, {1,3}, {2,3} });
}

BOOST_FIXTURE_TEST_CASE(ActnumNNCAtSeveralFaces, Fixture)
{
    Ewoms::NNC nnc;
    nnc.addNNC(2, 4, 1.0);   // inactive cell 2, no new connection
    nnc.addNNC(3, 4, 1.0);
    nnc.addNNC(2, 1, 1.0);
    nnc.addNNC(1, 2, 1.0);
    nnc.addNNC(1, 4, 1.0);   // new connection
    nnc.addNNC(1, 3, 1.0);   // new connection
    nnc.addNNC(999, 2, 1.0); // invalid
    nnc.addNNC(-1, 2, 1.0);  // invalid
    testCase("FIVE_ACTNUM.DATA", nnc, 4, 24 + 4, 20, { {0,1}, {1,2}, {1,3}, {2,3} });
}

BOOST_FIXTURE_TEST_CASE(NNCWithPINCH, Fixture)
{
    Ewoms::NNC nnc;
    testCase("FIVE_PINCH.DATA", nnc, 4, 24 + 1, 18 + 1, { {0,1}, {1,2}, {2,3} }, true);
}

BOOST_FIXTURE_TEST_CASE(NNCWithPINCHNOGAP, Fixture)
{
    Ewoms::NNC nnc;
    testCase("FIVE_PINCH_NOGAP.DATA", nnc, 4, 4 * 6 + 1, 2 * (4 + 5) + 1, { {0,1}, {1,2}, {2,3} }, true);
}

BOOST_FIXTURE_TEST_CASE(NNCWithPINCHNOGAP2, Fixture)
{
    Ewoms::NNC nnc;
    testCase("FIVE_PINCH_NOGAP2.DATA", nnc, 3, 3*6, 3*6-2, { {1,2} }, true);
}

BOOST_FIXTURE_TEST_CASE(NNCWithPINCHNOGAP3, Fixture)
{
    Ewoms::NNC nnc;
    testCase("FIVE_PINCH_NOGAP3.DATA", nnc, 2, 2*6, 2*6, { }, true);
}

BOOST_FIXTURE_TEST_CASE(NNCWithPINCHAndMore, Fixture)
{
    Ewoms::NNC nnc;
    nnc.addNNC(2, 4, 1.0);   // new connection
    nnc.addNNC(3, 4, 1.0);
    // The next causes trouble, and I have disabled it because I do not know
    // what should be the correct behaviour for that case.
    // nnc.addNNC(0, 2, 1.0);   // connection added from pinchout already
    nnc.addNNC(999, 2, 1.0); // invalid
    nnc.addNNC(-1, 2, 1.0);  // invalid
    testCase("FIVE_PINCH.DATA", nnc, 4, 24 + 2 + 1, 18 + 1, { {0,1}, {1,2}, {1,3}, {2,3} }, true);
}

BOOST_AUTO_TEST_SUITE_END()
