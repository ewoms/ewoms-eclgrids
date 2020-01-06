/*
 * This file is licensed under GPL3, see https://ewoms.org/
*/

#include <config.h>

#define NVERBOSE  // Suppress own messages when throw()ing

#define BOOST_TEST_MODULE TEST_UG
#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>

/* --- our own headers --- */
#include <algorithm>
#include <vector>
#include <ewoms/eclgrids/unstructuredgrid.h>
#include <ewoms/eclgrids/cornerpoint_grid.h>  /* compute_geometry */
#include <ewoms/eclgrids/gridmanager.hh>  /* compute_geometry */
#include <ewoms/eclgrids/gridhelpers.hh>
#include <ewoms/eclgrids/cpgpreprocess/preprocess.h>

#include <ewoms/eclgrids/utility/parserincludes.hh>

using namespace std;

BOOST_AUTO_TEST_CASE(Equal) {
    const std::string filename1 = "CORNERPOINT_ACTNUM.DATA";
    const char *deck2Data =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DXV\n"
        "10*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZV\n"
        "10*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "PORO\n"
        "   1000*0.15 /\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;

    Ewoms::Deck deck1 = parser.parseFile( filename1);
    Ewoms::EclipseState es1(deck1);

    Ewoms::Deck deck2 = parser.parseString( deck2Data);
    Ewoms::EclipseState es2(deck2);

    BOOST_CHECK( deck1.hasKeyword("ZCORN") );
    BOOST_CHECK( deck1.hasKeyword("COORD") );

    Ewoms::GridManager grid1(es1.getInputGrid());
    Ewoms::GridManager grid2(es2.getInputGrid());

    const UnstructuredGrid* cgrid1 = grid1.c_grid();
    const UnstructuredGrid* cgrid2 = grid2.c_grid();

    BOOST_CHECK( grid_equal( cgrid1 , cgrid1 ));
    BOOST_CHECK( grid_equal( cgrid2 , cgrid2 ));
    BOOST_CHECK( !grid_equal( cgrid1 , cgrid2 ));
}

// TODO This method might be obsolete after using EclipseState to generate grid
BOOST_AUTO_TEST_CASE(EqualEclipseGrid) {
    const std::string filename = "CORNERPOINT_ACTNUM.DATA";
    Ewoms::Parser parser;
    Ewoms::Deck deck = parser.parseFile( filename);
    Ewoms::EclipseState es(deck);
    auto grid = es.getInputGrid();

    Ewoms::GridManager gridM(es.getInputGrid());
    const UnstructuredGrid* cgrid1 = gridM.c_grid();
    struct UnstructuredGrid * cgrid2;
    {
        struct grdecl g;
        const auto& dimens = deck.getKeyword("DIMENS");
        const auto& coord = deck.getKeyword("COORD");
        const auto& zcorn = deck.getKeyword("ZCORN");
        const auto& actnum = deck.getKeyword("ACTNUM");

        g.dims[0] = dimens.getRecord(0).getItem("NX").get< int >(0);
        g.dims[1] = dimens.getRecord(0).getItem("NY").get< int >(0);
        g.dims[2] = dimens.getRecord(0).getItem("NZ").get< int >(0);

        g.coord  = coord.getSIDoubleData().data();
        g.zcorn  = zcorn.getSIDoubleData().data();
        g.actnum = actnum.getIntData().data();
        g.mapaxes = NULL;

        cgrid2 = create_grid_cornerpoint(&g , 0.0);
        if (!cgrid2)
            throw std::runtime_error("Failed to construct grid.");
    }

    BOOST_CHECK( grid_equal( cgrid1 , cgrid2 ));

    auto actnum = Ewoms::UgGridHelpers::createACTNUM(*cgrid1);
    BOOST_CHECK_EQUAL( actnum.size(), 500 );
    for (std::size_t i=0; i < 100; i++) {
        BOOST_CHECK_EQUAL(actnum[i + 200], 1);
        for (std::size_t j=0; j < 2; j++) {
            BOOST_CHECK_EQUAL(actnum[i + j * 100], 0);
            BOOST_CHECK_EQUAL(actnum[i + j * 100 + 300], 0);
        }
    }
    destroy_grid( cgrid2 );
}

BOOST_AUTO_TEST_CASE(TOPS_Fully_Specified) {
    const char *deck1Data =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 3 /\n"
        "GRID\n"
        "DX\n"
        "300*1000 /\n"
        "DY\n"
        "300*1000 /\n"
        "DZ\n"
        "100*20 100*30  100*50 /\n"
        "TOPS\n"
        "100*8325 /\n"
        "PORO\n"
        "   300*0.15 /\n"
        "EDIT\n"
        "\n";

    const char *deck2Data =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 3 /\n"
        "GRID\n"
        "DX\n"
        "300*1000 /\n"
        "DY\n"
        "300*1000 /\n"
        "DZ\n"
        "100*20 100*30  100*50 /\n"
        "TOPS\n"
        "100*8325 100*8345  100*8375/\n"
        "PORO\n"
        "   300*0.15 /\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    const Ewoms::Deck& deck1 = parser.parseString(deck1Data);
    const Ewoms::Deck& deck2 = parser.parseString(deck2Data);

    Ewoms::EclipseState es1(deck1);
    Ewoms::EclipseState es2(deck2);

    Ewoms::GridManager gridM1(es1.getInputGrid());
    Ewoms::GridManager gridM2(es2.getInputGrid());

    const UnstructuredGrid* cgrid1 = gridM1.c_grid();
    const UnstructuredGrid* cgrid2 = gridM2.c_grid();

    BOOST_CHECK(grid_equal(cgrid1, cgrid2));

    Ewoms::EclipseGrid grid = Ewoms::UgGridHelpers::createEclipseGrid( *cgrid1 , es1.getInputGrid( ) );
    auto actnum = Ewoms::UgGridHelpers::createACTNUM(*cgrid1);
    for (std::size_t g = 0; g < 300; g++)
        BOOST_CHECK_EQUAL(actnum[g], 1);
}
