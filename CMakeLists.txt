# -*- mode: cmake; tab-width: 2; indent-tabs-mode: t; truncate-lines: t
# vim: set filetype=cmake autoindent tabstop=2 shiftwidth=2 noexpandtab softtabstop=2 nowrap:
cmake_minimum_required(VERSION 3.10)

# set up project and specify the minimum cmake version
project("ewoms-eclgrids" C CXX)

# find the build system (i.e., ewoms-common) and set cmake's module path
find_package(ewoms-common REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${ewoms-common_MODULE_PATH})

# include the eWoms cmake macros
include(EwomsMacros)

# do most of the book-keeping required
ewoms_project()

# find the packages needed to compile the module
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# we want all features detected by the build system to be enabled,
# thank you!
dune_enable_all_packages()

# add source files from CMakeLists_files.cmake to
# library libewomsgrid.a and create executables
ewoms_recursive_add_library("ewomseclgrids" "ewoms/eclgrids")

# recursively mark all header files beneath the "ewoms" directory for
# installation.
ewoms_recusive_export_all_headers("ewoms")

# deal with the data required by the unit tests
ewoms_recusive_copy_testdata_to_builddir("tests/*.data" "tests/*.DATA")

ewoms_add_test(cartgrid SOURCES tests/test_cartgrid.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(cpgrid SOURCES tests/test_cpgrid.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(column_extract SOURCES tests/test_column_extract.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(distribution SOURCES tests/cpgrid/distribution_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(entityrep SOURCES tests/cpgrid/entityrep_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(entity SOURCES tests/cpgrid/entity_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(facetag SOURCES tests/cpgrid/facetag_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(geometry SOURCES tests/cpgrid/geometry_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(orientedentitytable SOURCES tests/cpgrid/orientedentitytable_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(partition_iterator SOURCES tests/cpgrid/partition_iterator_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(zoltan SOURCES tests/cpgrid/zoltan_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(geom2d SOURCES tests/test_geom2d.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(gridutilities SOURCES tests/test_gridutilities.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(minpvprocessor SOURCES tests/test_minpvprocessor.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(polyhedralgrid SOURCES tests/test_polyhedralgrid.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(p2pcommunicator SOURCES tests/p2pcommunicator_test.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(repairzcorn SOURCES tests/test_repairzcorn.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(sparsetable SOURCES tests/test_sparsetable.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(quadratures SOURCES tests/test_quadratures.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(compressed_cartesian_mapping SOURCES tests/test_compressed_cartesian_mapping.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(regionmapping SOURCES tests/test_regionmapping.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(ug SOURCES tests/test_ug.cc LIBRARIES "${Boost_LIBRARIES}")
ewoms_add_test(grid_nnc SOURCES tests/cpgrid/grid_nnc.cc LIBRARIES "${Boost_LIBRARIES}")

ewoms_recusive_copy_testdata("tests/*.DATA" "tests/*.data")

# finalize the eWoms project, e.g. generating config.h etc.
finalize_ewoms_project()
