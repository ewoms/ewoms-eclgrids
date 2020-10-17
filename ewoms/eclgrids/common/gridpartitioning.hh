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
#ifndef EWOMS_ECLGRIDSPARTITIONING_HEADER
#define EWOMS_ECLGRIDSPARTITIONING_HEADER

#include <vector>
#include <array>
#include <set>
#include <tuple>

#include <dune/common/parallel/mpihelper.hh>
namespace Dune
{

    class CpGrid;

    struct OrderByFirst
    {
        bool operator()(const std::pair<int,int>& o, const std::pair<int,int>& v)
        {
            return o.first < v.first;
        }
    };

    /// Partition a CpGrid based on (ijk) coordinates, with splitting to ensure that each partition is connected.
    /// @param[in] grid the grid to partition
    /// @param[in] initial_split the number of parts in which to partition the grid, in each cardinal direction.
    ///                          Their product is the expected number of partitions produced.
    /// @param[out] num_part the resulting number of partitions. This may be lower than expected,
    ///                      because of inactive cells, or higher than expected,
    ///                      because of splits to ensure connectedness.
    /// @param[out] cell_part a vector containing, for each cell, its partition number
    void partition(const CpGrid& grid,
                   const std::array<int, 3>& initial_split,
                   int& num_part,
                   std::vector<int>& cell_part,
                   bool recursive = false,
                   bool ensureConnectivity = true);

    /// \brief Adds a layer of overlap cells to a partitioning.
    /// \param[in] grid The grid that is partitioned.
    /// \param[in] cell_part a vector containing each cells partition number.
    /// \param[out] cell_overlap a vector of sets that contains for each cell all
    ///             the partition numbers that it is an overlap cell of.
    /// \param[in] mypart The partition number of the processor.
    /// \param[in] all Whether to compute the overlap for all partions or just the
    ///            one associated by mypart.
    void addOverlapLayer(const CpGrid& grid,
                         const std::vector<int>& cell_part,
                         std::vector<std::set<int> >& cell_overlap,
                         int mypart, int overlapLayers, bool all=false);

    /// \brief Adds a layer of overlap cells to a partitioning.
    /// \param[in] grid The grid that is partitioned.
    /// \param[in] cell_part a vector containing each cells partition number.
    /// \param[inout] exportList List indices to export, each entry is a tuple
    /// of global index, process rank (to export to), attribute on remote.
    /// \param[inout] importList List indices to import, each entry is a tuple
    /// of global index, process rank (to import from), attribute here, local
    /// index here
    /// \param[in] cc The communication object
    /// \param[in] addCornerCells Switch for adding corner cells to overlap layer.
    /// \param[in] trans The transmissibilities on cell faces. When trans[i]==0, no overlap is added.
    /// \param[in] layer Number of overlap layers
    int addOverlapLayer(const CpGrid& grid, const std::vector<int>& cell_part,
                        std::vector<std::tuple<int,int,char>>& exportList,
                        std::vector<std::tuple<int,int,char,int>>& importList,
                        const CollectiveCommunication<Dune::MPIHelper::MPICommunicator>& cc,
                        bool addCornerCells, const double* trans, int layers = 1);

} // namespace Dune

#endif // EWOMS_ECLGRIDSPARTITIONING_HEADER
