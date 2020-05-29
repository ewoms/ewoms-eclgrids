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

#include "config.h"

#include <ewoms/eclgrids/gridhelpers.hh>
#include <ewoms/eclgrids/gridmanager.hh>
#include <ewoms/eclgrids/unstructuredgrid.h>
#include <ewoms/eclgrids/cart_grid.h>
#include <ewoms/eclgrids/cornerpoint_grid.h>
#include <ewoms/eclgrids/minpvprocessor.hh>
#include <ewoms/eclio/errormacros.hh>

#include <ewoms/eclgrids/utility/parserincludes.hh>

#include <array>
#include <algorithm>
#include <numeric>

namespace Ewoms
{

#if HAVE_ECL_INPUT
    /// Construct a 3d corner-point grid from a deck.
    GridManager::GridManager(const Ewoms::EclipseGrid& inputGrid)
        : ug_(0)
    {
        initFromEclipseGrid(inputGrid, std::vector<double>());
    }

    GridManager::GridManager(const Ewoms::EclipseGrid& inputGrid,
                             const std::vector<double>& poreVolumes)
        : ug_(0)
    {
        initFromEclipseGrid(inputGrid, poreVolumes);
    }
#endif

    /// Construct a 2d cartesian grid with cells of unit size.
    GridManager::GridManager(int nx, int ny)
    {
        ug_ = create_grid_cart2d(nx, ny, 1.0, 1.0);
        if (!ug_) {
            EWOMS_THROW(std::runtime_error, "Failed to construct grid.");
        }
    }

    GridManager::GridManager(int nx, int ny,double dx, double dy)
    {
        ug_ = create_grid_cart2d(nx, ny, dx, dy);
        if (!ug_) {
            EWOMS_THROW(std::runtime_error, "Failed to construct grid.");
        }
    }

    /// Construct a 3d cartesian grid with cells of unit size.
    GridManager::GridManager(int nx, int ny, int nz)
    {
        ug_ = create_grid_cart3d(nx, ny, nz);
        if (!ug_) {
            EWOMS_THROW(std::runtime_error, "Failed to construct grid.");
        }
    }

    /// Construct a 3d cartesian grid with cells of size [dx, dy, dz].
    GridManager::GridManager(int nx, int ny, int nz,
                             double dx, double dy, double dz)
    {
        ug_ = create_grid_hexa3d(nx, ny, nz, dx, dy, dz);
        if (!ug_) {
            EWOMS_THROW(std::runtime_error, "Failed to construct grid.");
        }
    }

    /// Construct a grid from an input file.
    /// The file format used is currently undocumented,
    /// and is therefore only suited for internal use.
    GridManager::GridManager(const std::string& input_filename)
    {
        ug_ = read_grid(input_filename.c_str());
        if (!ug_) {
            EWOMS_THROW(std::runtime_error, "Failed to read grid from file " << input_filename);
        }
    }

    /// Destructor.
    GridManager::~GridManager()
    {
        destroy_grid(ug_);
    }

    /// Access the managed UnstructuredGrid.
    /// The method is named similarly to c_str() in std::string,
    /// to make it clear that we are returning a C-compatible struct.
    const UnstructuredGrid* GridManager::c_grid() const
    {
        return ug_;
    }

#if HAVE_ECL_INPUT
    // Construct corner-point grid from EclipseGrid.
    void GridManager::initFromEclipseGrid(const Ewoms::EclipseGrid& inputGrid,
                                          const std::vector<double>& poreVolumes)
    {
        struct grdecl g;

        g.dims[0] = inputGrid.getNX();
        g.dims[1] = inputGrid.getNY();
        g.dims[2] = inputGrid.getNZ();

        std::vector<double> mapaxes = inputGrid.getMAPAXES( );
        std::vector<double> coord = inputGrid.getCOORD( );
        std::vector<double> zcorn = inputGrid.getZCORN( );
        std::vector<int> actnum = inputGrid.getACTNUM(  );

        g.coord = coord.data();
        g.zcorn = zcorn.data();
        g.actnum = actnum.data();
        g.mapaxes = mapaxes.data();

        if (!poreVolumes.empty() && (inputGrid.getMinpvMode() != MinpvMode::ModeEnum::Inactive)) {
            MinpvProcessor mp(g.dims[0], g.dims[1], g.dims[2]);
            const std::vector<double>& minpvv  = inputGrid.getMinpvVector();
            const size_t cartGridSize = g.dims[0] * g.dims[1] * g.dims[2];
            std::vector<double> thickness(cartGridSize);
            for (size_t i = 0; i < cartGridSize; ++i) {
                thickness[i] = inputGrid.getCellThickness(i);
            }

            // The legacy code only supports the opmfil option
            bool opmfil = true; //inputGrid.getMinpvMode() == MinpvMode::OpmFIL;
            const double z_tolerance = inputGrid.isPinchActive() ? inputGrid.getPinchThresholdThickness() : 0.0;
            mp.process(thickness, z_tolerance, poreVolumes, minpvv, actnum, opmfil, zcorn.data());
        }

        const double z_tolerance = inputGrid.isPinchActive() ? inputGrid.getPinchThresholdThickness() : 0.0;
        ug_ = create_grid_cornerpoint(&g, z_tolerance);
        if (!ug_) {
            EWOMS_THROW(std::runtime_error, "Failed to construct grid.");
        }

        attach_zcorn_copy( ug_ , zcorn.data() );

    }

#endif

} // namespace Ewoms
