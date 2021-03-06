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

// Warning suppression for Dune includes.

#include <dune/common/unused.hh>
#include <ewoms/eclgrids/cpgrid.hh>
#include <ewoms/eclgrids/cpgrid/gridhelpers.hh>

#include <dune/grid/io/file/vtk/vtkwriter.hh>

#include <ewoms/eclgrids/cpgrid/dgfparser.hh>

#define DISABLE_DEPRECATED_METHOD_CHECK 1
#include <dune/grid/test/gridcheck.hh>

// Re-enable warnings.

#include <ewoms/eclgrids/utility/parserincludes.hh>

#include <iostream>

template <class GridView>
void testGridIteration( const GridView& gridView, const int nElem )
{
    typedef typename GridView::template Codim<0>::Iterator ElemIterator;
    typedef typename GridView::IntersectionIterator IsIt;
    typedef typename GridView::template Codim<0>::Geometry Geometry;

    int numElem = 0;
    ElemIterator elemIt = gridView.template begin<0>();
    ElemIterator elemEndIt = gridView.template end<0>();
    for (; elemIt != elemEndIt; ++elemIt) {
        const Geometry& elemGeom = elemIt->geometry();
        if (std::abs(elemGeom.volume() - 1.0) > 1e-8)
            std::cout << "element's " << numElem << " volume is wrong:"<<elemGeom.volume()<<"\n";

        typename Geometry::LocalCoordinate local( 0.5 );
        typename Geometry::GlobalCoordinate global = elemGeom.global( local );
        typename Geometry::GlobalCoordinate center = elemGeom.center();
        if( (center - global).two_norm() > 1e-6 )
        {
          std::cout << "center = " << center << " global( localCenter ) = " << global << std::endl;
        }

        int numIs = 0;
        IsIt isIt = gridView.ibegin(*elemIt);
        IsIt isEndIt = gridView.iend(*elemIt);
        for (; isIt != isEndIt; ++isIt, ++ numIs)
        {
            const auto& intersection = *isIt;
            const auto& isGeom = intersection.geometry();
            //std::cout << "Checking intersection id = " << localIdSet.id( intersection ) << std::endl;
            if (std::abs(isGeom.volume() - 1.0) > 1e-8)
                std::cout << "volume of intersection " << numIs << " of element " << numElem << " volume is wrong: " << isGeom.volume() << "\n";

            if (intersection.neighbor())
            {
              if( numIs != intersection.indexInInside() )
                  std::cout << "num iit = " << numIs << " indexInInside " << intersection.indexInInside() << std::endl;

              if (std::abs(intersection.outside().geometry().volume() - 1.0) > 1e-8)
                  std::cout << "outside element volume of intersection " << numIs << " of element " << numElem
                            << " volume is wrong: " << intersection.outside().geometry().volume() << std::endl;

              if (std::abs(intersection.inside().geometry().volume() - 1.0) > 1e-8)
                  std::cout << "inside element volume of intersection " << numIs << " of element " << numElem
                            << " volume is wrong: " << intersection.inside().geometry().volume() << std::endl;
            }
        }

        if (numIs != 2 * GridView::dimension )
            std::cout << "number of intersections is wrong for element " << numElem << "\n";

        ++ numElem;
    }

    if (numElem != nElem )
        std::cout << "number of elements is wrong: " << numElem << ", expected " << nElem << std::endl;
}

template <class Grid>
void testGrid(Grid& grid, const std::string& name, const size_t nElem, const size_t nVertices)
{
    typedef typename Grid::LeafGridView GridView;
    /*

    try {
      gridcheck( grid );
    }
    catch ( const Dune::Exception& e)
    {
      std::cerr << "Warning: " << e.what() << std::endl;
    }
*/
    std::cout << name << std::endl;

    testGridIteration( grid.leafGridView(), nElem );

    std::cout << "create vertex mapper\n";

#if DUNE_VERSION_NEWER(DUNE_COMMON, 2, 6)
    Dune::MultipleCodimMultipleGeomTypeMapper<GridView> mapper(grid.leafGridView(), Dune::mcmgVertexLayout());
#else
    Dune::MultipleCodimMultipleGeomTypeMapper<GridView, Dune::MCMGVertexLayout> mapper(grid.leafGridView());
#endif

    std::cout << "VertexMapper.size(): " << mapper.size() << "\n";
    if (static_cast<size_t>(mapper.size()) != nVertices ) {
        std::cout << "Wrong size of vertex mapper. Expected " << nVertices << "!" << std::endl;
        //std::abort();
    }

    // VTKWriter does not work with geometry type none at the moment
    if( true || grid.geomTypes( 0 )[ 0 ].isCube() )
    {
      std::cout << "create vtkWriter\n";
      typedef Dune::VTKWriter<GridView> VtkWriter;
      VtkWriter vtkWriter(grid.leafGridView());

      std::cout << "create cellData\n";
      int numElems = grid.size(0);
      std::vector<double> tmpData(numElems, 0.0);

      std::cout << "add cellData\n";
      vtkWriter.addCellData(tmpData, name);

      std::cout << "write data\n";
      vtkWriter.write(name, Dune::VTK::ascii);
    }

}

int main(int argc, char** argv )
{
    // initialize MPI
    Dune::MPIHelper::instance( argc, argv );

    // test CpGrid
    typedef Dune::CpGrid Grid;

#if HAVE_ECL_INPUT
    const char *deckString =
        "RUNSPEC\n"
        "METRIC\n"
        "DIMENS\n"
        "2 2 2 /\n"
        "GRID\n"
        "DXV\n"
        "2*1 /\n"
        "DYV\n"
        "2*1 /\n"
        "DZ\n"
        "8*1 /\n"
        "TOPS\n"
        "8*100.0 /\n";

    Ewoms::Parser parser;
    const auto deck = parser.parseString(deckString);
    std::vector<double> porv;

    Grid grid;
    const int* actnum = deck.hasKeyword("ACTNUM") ? deck.getKeyword("ACTNUM").getIntData().data() : nullptr;
    Ewoms::EclipseGrid ecl_grid(deck , actnum);

    grid.processEclipseFormat(&ecl_grid, false, false, false, porv);
    testGrid( grid, "CpGrid_ecl", 8, 27 );
#endif

    std::stringstream dgfFile;
    // create unit cube with 8 cells in each direction
    dgfFile << "DGF" << std::endl;
    dgfFile << "Interval" << std::endl;
    dgfFile << "0 0 0" << std::endl;
    dgfFile << "4 4 4" << std::endl;
    dgfFile << "4 4 4" << std::endl;
    dgfFile << "#" << std::endl;

    Dune::GridPtr< Grid > gridPtr( dgfFile );
    testGrid( *gridPtr, "CpGrid_dgf", 64, 125 );

    return 0;
}
