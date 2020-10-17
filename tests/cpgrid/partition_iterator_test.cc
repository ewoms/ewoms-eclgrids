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

#define BOOST_TEST_MODULE PartitionIteratorCpGridTests
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include <dune/common/version.hh>
#include <ewoms/eclgrids/cpgrid.hh>
#include <dune/grid/common/gridenums.hh>
#include <dune/geometry/referenceelements.hh>
#include <dune/common/fvector.hh>

#if HAVE_DUNE_GRID_CHECKS

#include <dune/grid/test/checkpartition.hh>

#endif

template<int codim>
void testPartitionIteratorsBasic(const Dune::CpGrid& grid, bool parallel)
{
    BOOST_REQUIRE((grid.leafbegin<codim,Dune::OverlapFront_Partition>()==
                      grid.leafbegin<codim,Dune::All_Partition>()));
    BOOST_REQUIRE((grid.leafend<codim,Dune::OverlapFront_Partition>()==
                      grid.leafend<codim,Dune::All_Partition>()));
    BOOST_REQUIRE((grid.leafbegin<codim,Dune::Ghost_Partition>()==
                   grid.leafend<codim,Dune::Ghost_Partition>()));
    if(!parallel)
    {
        // This is supposed to be a grid with only interior entities,
        // therefore the iterators for interior, interiorborder and all should match!
        BOOST_REQUIRE((grid.leafbegin<codim,Dune::Interior_Partition>()==
                   grid.leafbegin<codim,Dune::InteriorBorder_Partition>()));
        BOOST_REQUIRE((grid.leafbegin<codim,Dune::Interior_Partition>()==
                       grid.leafbegin<codim,Dune::All_Partition>()));
    }

}

template<int codim>
void testPartitionIteratorsOnSequentialGrid(const Dune::CpGrid& grid)
{
    typedef typename Dune::CpGrid::Traits::template Codim<codim>::template Partition<Dune::Interior_Partition>::LeafIterator ILeafIterator;
    typedef typename Dune::CpGrid::Traits::template Codim<codim>::template Partition<Dune::InteriorBorder_Partition>::LeafIterator IBLeafIterator;
    typedef typename Dune::CpGrid::Traits::template Codim<codim>::template Partition<Dune::All_Partition>::LeafIterator ALeafIterator;
    ILeafIterator iit=grid.leafbegin<codim,Dune::Interior_Partition>();
    IBLeafIterator ibit=grid.leafbegin<codim,Dune::InteriorBorder_Partition>();
    ALeafIterator ait=grid.leafbegin<codim,Dune::All_Partition>();
    while(iit!=grid.leafend<codim,Dune::Interior_Partition>())
    {
        BOOST_REQUIRE((*iit==*ibit));
        BOOST_REQUIRE((*iit==*ait));
        ++iit;
        ++ibit;
        ++ait;
    }
    BOOST_REQUIRE((ibit==grid.leafend<codim,Dune::InteriorBorder_Partition>()));
    BOOST_REQUIRE((ait==grid.leafend<codim,Dune::All_Partition>()));
}

BOOST_AUTO_TEST_CASE(partitionIteratorTest)
{
    int m_argc = boost::unit_test::framework::master_test_suite().argc;
    char** m_argv = boost::unit_test::framework::master_test_suite().argv;
    Dune::MPIHelper& helper = Dune::MPIHelper::instance(m_argc, m_argv);

    if(helper.rank()==0)
    {
        Dune::CpGrid grid;
        std::array<int, 3> dims={{2, 2, 2}};
        std::array<double, 3> size={{ 1.0, 1.0, 1.0}};

        grid.createCartesian(dims, size);
        testPartitionIteratorsBasic<0>(grid, false);
        testPartitionIteratorsOnSequentialGrid<0>(grid);
        testPartitionIteratorsBasic<1>(grid, false);
        testPartitionIteratorsOnSequentialGrid<1>(grid);
        testPartitionIteratorsBasic<3>(grid, false);
        testPartitionIteratorsOnSequentialGrid<3>(grid);
    }

    bool parallel =helper.size()>1;
    Dune::CpGrid grid;
    std::array<int, 3> dims={{10, 10, 10}};
    std::array<double, 3> size={{ 1.0, 1.0, 1.0}};

    grid.createCartesian(dims, size);
    grid.loadBalance();
    testPartitionIteratorsBasic<0>(grid, parallel);
    testPartitionIteratorsBasic<1>(grid, parallel);
    testPartitionIteratorsBasic<3>(grid, parallel);
    if(!parallel)
    {
        testPartitionIteratorsOnSequentialGrid<0>(grid);
        testPartitionIteratorsOnSequentialGrid<1>(grid);
        testPartitionIteratorsOnSequentialGrid<3>(grid);
    }

#if HAVE_DUNE_GRID_CHECKS
    checkPartitionType( grid.leafGridView() );
#endif // HAVE_DUNE_GRID_CHECKS

}

bool
init_unit_test_func()
{
    return true;
}

int main(int argc, char** argv)
{
    Dune::MPIHelper::instance(argc, argv);
    boost::unit_test::unit_test_main(&init_unit_test_func, argc, argv);
}
