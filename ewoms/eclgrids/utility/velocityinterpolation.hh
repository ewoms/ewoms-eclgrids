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
#ifndef EWOMS_VELOCITYINTERPOLATION_HH
#define EWOMS_VELOCITYINTERPOLATION_HH

#include <ewoms/eclgrids/utility/wachspresscoord.hh>
#include <vector>

struct UnstructuredGrid;

namespace Ewoms
{

    /// Abstract interface for velocity interpolation method classes.
    class VelocityInterpolationInterface
    {
    public:
        virtual ~VelocityInterpolationInterface();

        /// Set up fluxes for interpolation.
        /// \param[in]  flux   One signed flux per face in the grid.
        virtual void setupFluxes(const double* flux) = 0;

        /// Interpolate velocity.
        /// \param[in]  cell   Cell in which to interpolate.
        /// \param[in]  x      Coordinates of point at which to interpolate.
        ///                    Must be array of length grid.dimensions.
        /// \param[out] v      Interpolated velocity.
        ///                    Must be array of length grid.dimensions.
        virtual void interpolate(const int cell,
                                 const double* x,
                                 double* v) const = 0;
    };

    /// A constant velocity approximation. Will not actually interpolate
    /// unless the fluxes are consistent with a constant velocity.
    class VelocityInterpolationConstant : public VelocityInterpolationInterface
    {
    public:
        /// Constructor.
        /// \param[in]  grid   A grid.
        explicit VelocityInterpolationConstant(const UnstructuredGrid& grid);

        /// Set up fluxes for interpolation.
        /// \param[in]  flux   One signed flux per face in the grid.
        virtual void setupFluxes(const double* flux);

        /// Interpolate velocity.
        /// \param[in]  cell   Cell in which to interpolate.
        /// \param[in]  x      Coordinates of point at which to interpolate.
        ///                    Must be array of length grid.dimensions.
        /// \param[out] v      Interpolated velocity.
        ///                    Must be array of length grid.dimensions.
        virtual void interpolate(const int cell,
                                 const double* x,
                                 double* v) const;
    private:
        const UnstructuredGrid& grid_;
        const double* flux_;
    };

    /// Interpolate velocity using the extended CVI scheme:
    /// compute a corner velocity for each cell corner that
    /// is consistent with fluxes of adjacent faces, then
    /// interpolate with generalized barycentric coordinates.
    class VelocityInterpolationECVI : public VelocityInterpolationInterface
    {
    public:
        /// Constructor.
        /// \param[in]  grid   A grid.
        explicit VelocityInterpolationECVI(const UnstructuredGrid& grid);

        /// Set up fluxes for interpolation.
        /// \param[in]  flux   One signed flux per face in the grid.
        virtual void setupFluxes(const double* flux);

        /// Interpolate velocity.
        /// \param[in]  cell   Cell in which to interpolate.
        /// \param[in]  x      Coordinates of point at which to interpolate.
        ///                    Must be array of length grid.dimensions.
        /// \param[out] v      Interpolated velocity.
        ///                    Must be array of length grid.dimensions.
        virtual void interpolate(const int cell,
                                 const double* x,
                                 double* v) const;
    private:
        WachspressCoord bcmethod_;
        const UnstructuredGrid& grid_;
        mutable std::vector<double> bary_coord_;
        std::vector<double> corner_velocity_; // size = dim * #corners
    };

} // namespace Ewoms

#endif // EWOMS_VELOCITYINTERPOLATION_HH
