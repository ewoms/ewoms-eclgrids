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
#ifndef EWOMS_DEFAULTGEOMETRYPOLICY_HEADER
#define EWOMS_DEFAULTGEOMETRYPOLICY_HEADER

#include "geometry.hh"
#include "entityrep.hh"

namespace Dune
{
    namespace cpgrid
    {
        /// @brief
        /// @todo Doc me!
        class DefaultGeometryPolicy
        {
            friend class CpGridData;
        public:
            /// @brief
            /// @todo Doc me
            DefaultGeometryPolicy()
            {
            }

            /// @brief
            /// @todo Doc me
            /// @param
            DefaultGeometryPolicy(const EntityVariable<cpgrid::Geometry<3, 3>, 0>& cell_geom,
                                  const EntityVariable<cpgrid::Geometry<2, 3>, 1>& face_geom,
                                  const EntityVariable<cpgrid::Geometry<0, 3>, 3>& point_geom)
                : cell_geom_(cell_geom), face_geom_(face_geom), point_geom_(point_geom)
            {
            }

            /// @brief
            /// @todo Doc me!
            /// @tparam
            /// @param
            /// @return
            template <int codim>
            const EntityVariable<cpgrid::Geometry<3 - codim, 3>, codim>& geomVector() const
            {
                static_assert(codim != 2, "");
                return geomVector(std::integral_constant<int,codim>());
            }

        private:
            /// \brief Get cell geometry
            const EntityVariable<cpgrid::Geometry<3, 3>, 0>& geomVector(const std::integral_constant<int, 0>&) const
            {
                return cell_geom_;
            }
            /// \brief Get cell geometry
            EntityVariable<cpgrid::Geometry<3, 3>, 0>& geomVector(const std::integral_constant<int, 0>&)
            {
                return cell_geom_;
            }
            /// \brief Get face geometry
            const EntityVariable<cpgrid::Geometry<2, 3>, 1>& geomVector(const std::integral_constant<int, 1>&) const
            {
                return face_geom_;
            }
            /// \brief Get face geometry
            EntityVariable<cpgrid::Geometry<2, 3>, 1>& geomVector(const std::integral_constant<int, 1>&)
            {
                return face_geom_;
            }

            /// \brief Get point geometry
            template<int codim>
            const EntityVariable<cpgrid::Geometry<0, 3>, 3>& geomVector(const std::integral_constant<int, codim>&) const
            {
                static_assert(codim==3, "Codim has to be 3");
                return point_geom_;
            }/// \brief Get point geometry
            template<int codim>
            EntityVariable<cpgrid::Geometry<0, 3>, 3>& geomVector(const std::integral_constant<int, codim>&)
            {
                static_assert(codim==3, "Codim has to be 3");
                return point_geom_;
            }
            EntityVariable<cpgrid::Geometry<3, 3>, 0> cell_geom_;
            EntityVariable<cpgrid::Geometry<2, 3>, 1> face_geom_;
            EntityVariable<cpgrid::Geometry<0, 3>, 3> point_geom_;
        };

    } // namespace cpgrid
} // namespace Dune

#endif // EWOMS_DEFAULTGEOMETRYPOLICY_HEADER
