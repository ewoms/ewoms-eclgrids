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
#ifndef DUNE_CPGRID_ZOLTANPARTITION_HEADER
#define DUNE_CPGRID_ZOLTANPARTITION_HEADER

#include <unordered_set>

#include <ewoms/eclgrids/cpgrid.hh>
#include <ewoms/eclgrids/common/zoltangraphfunctions.hh>

#if defined(HAVE_ZOLTAN) && defined(HAVE_MPI)
namespace Dune
{
namespace cpgrid
{
/// \brief Partition a CpGrid using Zoltan
///
/// This function will extract Zoltan's graph information
/// from the grid, and the wells and use it to partition the grid.
/// In case the global grid is available on all processes, it
/// will nevertheless only use the information on the root process
/// to partition it as Zoltan cannot identify this situation.
/// @param grid The grid to partition
/// @param wells The wells of the eclipse If null wells will be neglected.
/// @param transmissibilities The transmissibilities associated with the
///             faces
/// @paramm cc  The MPI communicator to use for the partitioning.
///             The will be partitioned among the partiticipating processes.
/// @param edgeWeightMethod The method used to calculate the weights associated
///             with the edges of the graph (uniform, transmissibilities, log thereof)
/// @param root The process number that holds the global grid.
/// @return A tuple consisting of a vector that contains for each local cell of the original grid the
///         the number of the process that owns it after repartitioning,
///         a vector containing a pair of name  and a boolean indicating whether this well has
///         perforated cells local to the process of all wells,
///         vector containing information for each exported cell (global id
///         of cell, process id to send to, attribute there), and a vector containing
///         information for each imported cell (global index, process id that sends, attribute here, local index
///         here)
std::tuple<std::vector<int>,std::vector<std::pair<std::string,bool>>,
           std::vector<std::tuple<int,int,char> >,
           std::vector<std::tuple<int,int,char,int> >  >
zoltanGraphPartitionGridOnRoot(const CpGrid& grid,
                               const std::vector<EwomsEclWellType> * wells,
                               const double* transmissibilities,
                               const CollectiveCommunication<MPI_Comm>& cc,
                               EdgeWeightMethod edgeWeightsMethod, int root);

/// \brief Partition a CpGrid using Zoltan serially only on rank 0
///
/// This function will extract Zoltan's graph information
/// from the grid, and the wells and use it to partition the grid.
/// In case the global grid is available on all processes, it
/// will nevertheless only use the information on the root process
/// to partition it as Zoltan cannot identify this situation.
/// @param grid The grid to partition
/// @param wells The wells of the eclipse If null wells will be neglected.
/// @param transmissibilities The transmissibilities associated with the
///             faces
/// @paramm cc  The MPI communicator to use for the partitioning.
///             The will be partitioned among the partiticipating processes.
/// @param edgeWeightMethod The method used to calculate the weights associated
///             with the edges of the graph (uniform, transmissibilities, log thereof)
/// @param root The process number that holds the global grid.
/// @return A tuple consisting of a vector that contains for each local cell of the original grid the
///         the number of the process that owns it after repartitioning,
///         a set of names of wells that should be defunct in a parallel
///         simulation, vector containing information for each exported cell (global id
///         of cell, process id to send to, attribute there), and a vector containing
///         information for each imported cell (global index, process id that sends, attribute here, local index
///         here)
///
/// @note This function will only do *serial* partioning.
std::tuple<std::vector<int>, std::vector<std::pair<std::string,bool>>,
           std::vector<std::tuple<int,int,char> >,
           std::vector<std::tuple<int,int,char,int> >  >
zoltanSerialGraphPartitionGridOnRoot(const CpGrid& grid,
                               const std::vector<EwomsEclWellType> * wells,
                               const double* transmissibilities,
                               const CollectiveCommunication<MPI_Comm>& cc,
                               EdgeWeightMethod edgeWeightsMethod, int root);
}
}
#endif // HAVE_ZOLTAN
#endif // header guard
