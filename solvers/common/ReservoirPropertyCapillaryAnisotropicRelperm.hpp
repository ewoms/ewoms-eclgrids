//===========================================================================
//
// File: ReservoirPropertyCapillaryAnisotropicRelperm.hpp
//
// Created: Fri Oct 23 08:12:21 2009
//
// Author(s): Atgeirr F Rasmussen <atgeirr@sintef.no>
//            B�rd Skaflestad     <bard.skaflestad@sintef.no>
//
// $Date$
//
// $Revision$
//
//===========================================================================

/*
  Copyright 2009 SINTEF ICT, Applied Mathematics.
  Copyright 2009 Statoil ASA.

  This file is part of The Open Reservoir Simulator Project (OpenRS).

  OpenRS is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OpenRS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OpenRS.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENRS_RESERVOIRPROPERTYCAPILLARYANISOTROPICRELPERM_HEADER
#define OPENRS_RESERVOIRPROPERTYCAPILLARYANISOTROPICRELPERM_HEADER

#include <dune/solvers/common/RockAnisotropicRelperm.hpp>
#include <dune/solvers/common/ReservoirPropertyCommon.hpp>

namespace Dune
{


    /// @brief A wrapper for a tensor.
    template <int dim>
    struct TensorMobility
    {
	FullMatrix<double, SharedData, COrdering> mob;

	TensorMobility()
	    : mob(dim, dim, tensor_storage_.data())
	{
	}
	void setToAverage(const TensorMobility& m1, const TensorMobility& m2)
	{
	    for (int i = 0; i < dim*dim; ++i) {
		tensor_storage_[i] = 0.5*(m1.tensor_storage_[i] + m2.tensor_storage_[i]);
	    }
	}
	void setToSum(const TensorMobility& m1, const TensorMobility& m2)
	{
	    for (int i = 0; i < dim*dim; ++i) {
		tensor_storage_[i] = m1.tensor_storage_[i] + m2.tensor_storage_[i];
	    }
	}
	void setToInverse(const TensorMobility& m)
	{
	    tensor_storage_ = m.tensor_storage_;
	    invert(mob);
	}
	template <class Vec>
	Vec multiply(const Vec& v)
	{
	    return prod(mob, v);
	}
    private:
	// If allowing copy, remember to set mob.data() properly.
	TensorMobility(const TensorMobility&);
	TensorMobility& operator=(const TensorMobility&);
	boost::array<double, dim*dim> tensor_storage_;
    };


    /// @brief A property class for incompressible two-phase flow.
    /// @tparam dim the dimension of the space, used for giving permeability tensors the right size.
    template <int dim>
    class ReservoirPropertyCapillaryAnisotropicRelperm
	: public ReservoirPropertyCommon<dim, ReservoirPropertyCapillaryAnisotropicRelperm<dim>, RockAnisotropicRelperm>
    {
    public:
	/// @brief The (tensorial) mobility type.
	typedef TensorMobility<dim> Mobility;

	/// @brief Anisotropic phase mobility.
	/// @param phase_index phase for which to compute mobility.
	/// @param cell_index index of a grid cell.
	/// @param saturation a saturation value.
	/// @param[out] phase_mob anisotropic phase mobility tensor at the given cell and saturation.
	template <class MatrixType>
	void phaseMobility(int phase_index,
			   int cell_index,
			   double saturation,
			   MatrixType& phase_mob) const;

	void computeCflFactors();

    private:
	typedef ReservoirPropertyCommon<dim, ReservoirPropertyCapillaryAnisotropicRelperm<dim>, RockAnisotropicRelperm> Super;
    };


} // namespace Dune

#include "ReservoirPropertyCapillaryAnisotropicRelperm_impl.hpp"


#endif // OPENRS_RESERVOIRPROPERTYCAPILLARYANISOTROPICRELPERM_HEADER