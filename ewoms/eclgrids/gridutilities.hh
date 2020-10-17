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
#ifndef EWOMS_ECLGRIDSUTILITIES_HH
#define EWOMS_ECLGRIDSUTILITIES_HH

#include <ewoms/eclgrids/unstructuredgrid.h>
#include <ewoms/eclgrids/utility/sparsetable.hh>

namespace Ewoms
{

    /// For each cell, find indices of all cells sharing a vertex with it.
    /// \param[in] grid    A grid object.
    /// \return            A table of neighbour cell-indices by cell.
    SparseTable<int> cellNeighboursAcrossVertices(const UnstructuredGrid& grid);

    /// For each cell, order the (cell) neighbours counterclockwise.
    /// \param[in] grid    A 2d grid object.
    /// \param[in, out] nb A cell-cell neighbourhood table, such as from vertexNeighbours().
    void orderCounterClockwise(const UnstructuredGrid& grid,
                               SparseTable<int>& nb);

} // namespace Ewoms

#endif // EWOMS_ECLGRIDSUTILITIES_HH
