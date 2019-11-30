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
#ifndef DUNE_POLYHEDRALGRID_GRIDHELPERS_HH
#define DUNE_POLYHEDRALGRID_GRIDHELPERS_HH

#include <ewoms/eclgrids/gridhelpers.hh>
#include <ewoms/eclgrids/polyhedralgrid.hh>

namespace Ewoms
{
namespace UgGridHelpers
{

template<int dim, int dimworld>
struct CellCentroidTraits< Dune::PolyhedralGrid< dim, dimworld > >
  : public CellCentroidTraits<UnstructuredGrid>
{
};

template<int dim, int dimworld>
struct CellVolumeIteratorTraits< Dune::PolyhedralGrid< dim, dimworld > >
  : public CellVolumeIteratorTraits<UnstructuredGrid>
{
};

template<int dim, int dimworld>
struct FaceCentroidTraits< Dune::PolyhedralGrid< dim, dimworld > >
 : public FaceCentroidTraits< UnstructuredGrid >
{
};

template<int dim, int dimworld>
struct Cell2FacesTraits< Dune::PolyhedralGrid< dim, dimworld > >
 : public Cell2FacesTraits<UnstructuredGrid>
{
};

template<int dim, int dimworld>
struct Face2VerticesTraits< Dune::PolyhedralGrid< dim, dimworld > >
 : public Face2VerticesTraits<UnstructuredGrid>
{
};

template<int dim, int dimworld>
struct FaceCellTraits< Dune::PolyhedralGrid< dim, dimworld > >
 : public FaceCellTraits<UnstructuredGrid>
{
};

} // end namespace UGGridHelpers
} // end namespace Ewoms
#endif
