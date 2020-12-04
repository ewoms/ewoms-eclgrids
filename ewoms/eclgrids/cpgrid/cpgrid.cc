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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_MPI
#include "mpi.h"
#endif

#include "../cpgrid.hh"
#include "cpgriddata.hh"
#include <ewoms/eclgrids/common/zoltanpartition.hh>
#include <ewoms/eclgrids/common/zoltangraphfunctions.hh>
#include <ewoms/eclgrids/common/gridpartitioning.hh>
#include <ewoms/eclgrids/common/wellconnections.hh>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <tuple>

namespace
{

#if HAVE_MPI

using AttributeSet = Dune::cpgrid::CpGridData::AttributeSet;

template<typename Tuple, bool first>
void reserveInterface(const std::vector<Tuple>& list, Dune::CpGrid::InterfaceMap& interface,
                      const std::integral_constant<bool, first>&)
{
    std::map<int, std::size_t> proc_to_no_cells;
    for(const auto& entry: list)
    {
        ++proc_to_no_cells[std::get<1>(entry)];
    }
    for(const auto& proc: proc_to_no_cells)
    {
        auto& entry = interface[proc.first];
        if ( first )
            entry.first.reserve(proc.second);
        else
            entry.second.reserve(proc.second);
    }
}

#ifdef HAVE_ZOLTAN
void setupSendInterface(const std::vector<std::tuple<int, int, char> >& list, Dune::CpGrid::InterfaceMap& interface)
{
    reserveInterface(list, interface, std::integral_constant<bool, true>());
    int cellIndex=-1;
    int oldIndex = std::numeric_limits<int>::max();
    for(const auto& entry: list)
    {
        auto index = std::get<0>(entry);
        assert(oldIndex == std::numeric_limits<int>::max() || index >= oldIndex);

        if (index != oldIndex )
        {
            oldIndex = index;
            ++cellIndex;
        }
        interface[std::get<1>(entry)].first.add(cellIndex);
    }
}

void setupRecvInterface(const std::vector<std::tuple<int, int, char, int> >& list, Dune::CpGrid::InterfaceMap& interface)
{
    reserveInterface(list, interface, std::integral_constant<bool, false>());
    for(const auto& entry: list)
    {
        auto index = std::get<3>(entry);
        interface[std::get<1>(entry)].second.add(index);
    }
}
#endif // HAVE_ZOLTAN
#endif // HAVE_MPI
}

namespace Dune
{

    CpGrid::CpGrid()
        : data_( new cpgrid::CpGridData(*this)),
          current_view_data_(data_.get()),
          distributed_data_(),
          cell_scatter_gather_interfaces_(new InterfaceMap),
          point_scatter_gather_interfaces_(new InterfaceMap),
          global_id_set_(*current_view_data_)
    {}

    CpGrid::CpGrid(MPIHelper::MPICommunicator  comm)
        : data_( new cpgrid::CpGridData(comm)),
          current_view_data_(data_.get()),
          distributed_data_(),
          cell_scatter_gather_interfaces_(new InterfaceMap),
          point_scatter_gather_interfaces_(new InterfaceMap),
          global_id_set_(*current_view_data_)
    {}

std::pair<bool, std::vector<std::pair<std::string,bool> > >
CpGrid::scatterGrid(EdgeWeightMethod method,
                    [[maybe_unused]] bool ownersFirst,
                    const std::vector<cpgrid::EwomsEclWellType> * wells,
                    [[maybe_unused]] bool serialPartitioning,
                    const double* transmissibilities,
                    [[maybe_unused]] bool addCornerCells,
                    int overlapLayers,
                    [[maybe_unused]] bool useZoltan)
{
    // Silence any unused argument warnings that could occur with various configurations.
    static_cast<void>(wells);
    static_cast<void>(transmissibilities);
    static_cast<void>(overlapLayers);
    static_cast<void>(method);
    if(distributed_data_)
    {
        std::cerr<<"There is already a distributed version of the grid."
                 << " Maybe scatterGrid was called before?"<<std::endl;
        return std::make_pair(false, std::vector<std::pair<std::string,bool> >());
    }

#if HAVE_MPI
    auto& cc = data_->ccobj_;

    if (cc.size() > 1)
    {
        std::vector<int> cell_part;
        std::vector<std::pair<std::string,bool>> wells_on_proc;
        std::vector<std::tuple<int,int,char>> exportList;
        std::vector<std::tuple<int,int,char,int>> importList;

        if (useZoltan)
        {
#ifdef HAVE_ZOLTAN
            std::tie(cell_part, wells_on_proc, exportList, importList)
                = serialPartitioning
                ? cpgrid::zoltanSerialGraphPartitionGridOnRoot(*this, wells, transmissibilities, cc, method, 0)
                : cpgrid::zoltanGraphPartitionGridOnRoot(*this, wells, transmissibilities, cc, method, 0);
#else
            EWOMS_THROW(std::runtime_error, "Parallel runs depend on ZOLTAN if useZoltan is true. Please install!");
#endif // HAVE_ZOLTAN
        }
        else
        {
        std::vector<int> exportGlobalIds;
        std::vector<int> exportLocalIds;
        std::vector<int> exportToPart;
        std::vector<int> importGlobalIds;
        std::size_t numExport = 0;
        int root = 0;

        if (cc.rank() == root)
        {
            std::vector<int> parts(current_view_data_->global_cell_.size());
            int  numParts=-1;
            std::array<int, 3> initialSplit;
            initialSplit[1]=initialSplit[2]=std::pow(cc.size(), 1.0/3.0);
            initialSplit[0]=cc.size()/(initialSplit[1]*initialSplit[2]);
            partition(*this, initialSplit, numParts, parts, false, false);
            // Create export lists as from Zoltan output, do not include part 0!
            exportGlobalIds.reserve(numCells());
            exportLocalIds.reserve(numCells());
            exportToPart.reserve(numCells());
            for (auto cell = leafbegin<0>(), cellEnd = leafend<0>();
                 cell != cellEnd; ++cell)
            {
                const auto& gid = globalIdSet().id(*cell);
                const auto& lid = localIdSet().id(*cell);
                const auto& index = leafIndexSet().index(cell);
                const auto& part = parts[index];
                if (part != 0 )
                {
                    exportGlobalIds.push_back(gid);
                    exportLocalIds.push_back(lid);
                    exportToPart.push_back(part);
                    ++numExport;
                }
            }
        }
        int numImport = 0;
        std::tie(numImport, importGlobalIds) =
            cpgrid::scatterExportInformation(numExport, exportGlobalIds.data(),
                                             exportToPart.data(), 0, cc);
        const bool allowDistributedWells = false;
        std::unique_ptr<cpgrid::CombinedGridWellGraph> gridAndWells;
        if (wells && !allowDistributedWells)
        {
            bool partitionIsEmpty = (size(0) == 0);
            gridAndWells.reset(new cpgrid::CombinedGridWellGraph(*this,
                                                       wells,
                                                       transmissibilities,
                                                       partitionIsEmpty,
                                                       method));
        }
        std::tie(cell_part, wells_on_proc, exportList, importList) =
            cpgrid::makeImportAndExportLists(*this, comm(),
                                             wells,
                                             gridAndWells.get(),
                                             root,
                                             numExport,
                                             numImport,
                                             exportLocalIds.data(),
                                             exportGlobalIds.data(),
                                             exportToPart.data(),
                                             importGlobalIds.data(),
                                             allowDistributedWells);
        }

        // first create the overlap
        // map from process to global cell indices in overlap
        std::map<int,std::set<int> > overlap;
        auto noImportedOwner = addOverlapLayer(*this, cell_part, exportList, importList, cc, addCornerCells,
                                               transmissibilities);
        // importList contains all the indices that will be here.
        auto compareImport = [](const std::tuple<int,int,char,int>& t1,
                                const std::tuple<int,int,char,int>&t2)
                             {
                                 return std::get<0>(t1) < std::get<0>(t2);
                             };

        if ( ! ownersFirst )
        {
            // merge owner and overlap sorted by global index
            std::inplace_merge(importList.begin(), importList.begin()+noImportedOwner,
                               importList.end(), compareImport);
        }
        // assign local indices
        int localIndex = 0;
        for(auto&& entry: importList)
            std::get<3>(entry) = localIndex++;

        if ( ownersFirst )
        {
            // merge owner and overlap sorted by global index
            std::inplace_merge(importList.begin(), importList.begin()+noImportedOwner,
                               importList.end(), compareImport);
        }

        int procsWithZeroCells{};

        if (cc.rank()==0)
        {
            // Print some statistics without communication
            std::vector<int> ownedCells(cc.size(), 0);
            std::vector<int> overlapCells(cc.size(), 0);
            for (const auto& entry: exportList)
            {
                if(std::get<2>(entry) == AttributeSet::owner)
                {
                    ++ownedCells[std::get<1>(entry)];
                }
                else
                {
                    ++overlapCells[std::get<1>(entry)];
                }
            }

            for(const auto& cellsOnProc: ownedCells)
            {
                procsWithZeroCells += (cellsOnProc == 0);
            }
            std::ostringstream ostr;
            ostr << "\nLoad balancing distributes " << data_->size(0)
                 << " active cells on " << cc.size() << " processes as follows:\n";
            ostr << "  rank   owned cells   overlap cells   total cells\n";
            ostr << "--------------------------------------------------\n";
            for (int i = 0; i < cc.size(); ++i) {
                ostr << std::setw(6) << i
                     << std::setw(14) << ownedCells[i]
                     << std::setw(16) << overlapCells[i]
                     << std::setw(14) << ownedCells[i] + overlapCells[i] << "\n";
            }
            ostr << "--------------------------------------------------\n";
            ostr << "   sum";
            auto sumOwned = std::accumulate(ownedCells.begin(), ownedCells.end(), 0);
            ostr << std::setw(14) << sumOwned;
            auto sumOverlap = std::accumulate(overlapCells.begin(), overlapCells.end(), 0);
            ostr << std::setw(16) << sumOverlap;
            ostr << std::setw(14) << (sumOwned + sumOverlap) << "\n";
            Ewoms::OpmLog::info(ostr.str());
        }

        procsWithZeroCells = cc.sum(procsWithZeroCells);

        if (procsWithZeroCells) {
            if (cc.rank()==0)
            {
                EWOMS_THROW(std::runtime_error, "At least one process has zero cells. Aborting.");
            }
            else
            {
                EWOMS_THROW_NOLOG(std::runtime_error, "At least one process has zero cells. Aborting.");
            }
        }

        distributed_data_.reset(new cpgrid::CpGridData(cc));
        distributed_data_->setUniqueBoundaryIds(data_->uniqueBoundaryIds());
        // Just to be sure we assume that only master knows
        cc.broadcast(&distributed_data_->use_unique_boundary_ids_, 1, 0);

        // Create indexset
        distributed_data_->cell_indexset_.beginResize();
        for(const auto& entry: importList)
        {
            distributed_data_->cell_indexset_.add(std::get<0>(entry), ParallelIndexSet::LocalIndex(std::get<3>(entry), AttributeSet(std::get<2>(entry)), true));
        }
        distributed_data_->cell_indexset_.endResize();
        // add an interface for gathering/scattering data with communication
        // forward direction will be scatter and backward gather
        // Interface will communicate from owner to all
        setupSendInterface(exportList, *cell_scatter_gather_interfaces_);
        setupRecvInterface(importList, *cell_scatter_gather_interfaces_);

        distributed_data_->distributeGlobalGrid(*this,*this->current_view_data_, cell_part);
        global_id_set_.insertIdSet(*distributed_data_);

        current_view_data_ = distributed_data_.get();
        return std::make_pair(true, wells_on_proc);
    }
    else
    {
        std::cerr << "CpGrid::scatterGrid() only makes sense in a parallel run. "
                  << "This run only uses one process.\n";
        return std::make_pair(false, std::vector<std::pair<std::string,bool>>());
    }
#else // !HAVE_MPI
    std::cerr << "CpGrid::scatterGrid() is non-trivial only with "
              << "MPI support and if the target Dune platform is "
              << "sufficiently recent.\n";
    return std::make_pair(false, std::vector<std::pair<std::string,bool>>());
#endif
}

    void CpGrid::createCartesian(const std::array<int, 3>& dims,
                                 const std::array<double, 3>& cellsize)
    {
        if ( current_view_data_->ccobj_.rank() != 0 )
        {
            // global grid only on rank 0
            current_view_data_->ccobj_.broadcast(current_view_data_->logical_cartesian_size_.data(),
                                                 current_view_data_->logical_cartesian_size_.size(),
                                                 0);
            return;
        }

        // Make the grdecl format arrays.
        // Pillar coords.
        std::vector<double> coord;
        coord.reserve(6*(dims[0] + 1)*(dims[1] + 1));
        double bot = 0.0;
        double top = dims[2]*cellsize[2];
        // i runs fastest for the pillars.
        for (int j = 0; j < dims[1] + 1; ++j) {
            double y = j*cellsize[1];
            for (int i = 0; i < dims[0] + 1; ++i) {
                double x = i*cellsize[0];
                double pillar[6] = { x, y, bot, x, y, top };
                coord.insert(coord.end(), pillar, pillar + 6);
            }
        }
        std::vector<double> zcorn(8*dims[0]*dims[1]*dims[2]);
        const int num_per_layer = 4*dims[0]*dims[1];
        double* offset = &zcorn[0];
        for (int k = 0; k < dims[2]; ++k) {
            double zlow = k*cellsize[2];
            std::fill(offset, offset + num_per_layer, zlow);
            offset += num_per_layer;
            double zhigh = (k+1)*cellsize[2];
            std::fill(offset, offset + num_per_layer, zhigh);
            offset += num_per_layer;
        }
        std::vector<int> actnum(dims[0]*dims[1]*dims[2], 1);

        // Process them.
        grdecl g;
        g.dims[0] = dims[0];
        g.dims[1] = dims[1];
        g.dims[2] = dims[2];
        g.coord = &coord[0];
        g.zcorn = &zcorn[0];
        g.actnum = &actnum[0];
        current_view_data_->processEclipseFormat(g, {}, 0.0, false, false);
        // global grid only on rank 0
        current_view_data_->ccobj_.broadcast(current_view_data_->logical_cartesian_size_.data(),
                                             current_view_data_->logical_cartesian_size_.size(),
                                             0);
    }

    void CpGrid::readSintefLegacyFormat(const std::string& grid_prefix)
    {
        if ( current_view_data_->ccobj_.rank() == 0 )
        {
            current_view_data_->readSintefLegacyFormat(grid_prefix);
        }
        current_view_data_->ccobj_.broadcast(current_view_data_->logical_cartesian_size_.data(),
                                             current_view_data_->logical_cartesian_size_.size(),
                                             0);
    }
    void CpGrid::writeSintefLegacyFormat(const std::string& grid_prefix) const
    {
        // Only rank 0 has the full data. Use that for writing.
        if ( current_view_data_->ccobj_.rank() == 0 )
        {
            data_->writeSintefLegacyFormat(grid_prefix);
        }
    }

#if HAVE_ECL_INPUT
    void CpGrid::processEclipseFormat(const Ewoms::EclipseGrid* ecl_grid,
                                      bool periodic_extension,
                                      bool turn_normals, bool clip_z,
                                      const std::vector<double>& poreVolume,
                                      const Ewoms::NNC& nncs)
    {
        current_view_data_->processEclipseFormat(ecl_grid, periodic_extension,
                                                 turn_normals, clip_z,
                                                 poreVolume, nncs);
        current_view_data_->ccobj_.broadcast(current_view_data_->logical_cartesian_size_.data(),
                                             current_view_data_->logical_cartesian_size_.size(),
                                             0);
    }
#endif

    void CpGrid::processEclipseFormat(const grdecl& input_data, double z_tolerance,
                                      bool remove_ij_boundary, bool turn_normals)
    {
        current_view_data_->processEclipseFormat(input_data, {}, z_tolerance, remove_ij_boundary, turn_normals);
        current_view_data_->ccobj_.broadcast(current_view_data_->logical_cartesian_size_.data(),
                                             current_view_data_->logical_cartesian_size_.size(),
                                             0);
    }

} // namespace Dune
