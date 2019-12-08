//===========================================================================
//
// File: Intersection.hpp
//
// Created: Tue Jun  9 11:17:13 2009
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

#ifndef EWOMS_INTERSECTION_HEADER
#define EWOMS_INTERSECTION_HEADER

#include <dune/grid/common/gridenums.hh>

#include <ewoms/eclgrids/utility/errormacros.hh>

// The next statement is a layering violation: we only #include
// preprocess.h to get at its "enum face_tag" definition.  Enum
// face_tag is needed in method Intersection::boundaryId().  This hack
// is in dire need of a better solution!
#include <ewoms/eclgrids/cpgpreprocess/preprocess.h>

#include "geometry.hh"
#include "orientedentitytable.hh"
namespace Dune
{
    namespace cpgrid
    {
    template<int>
    class Entity;
    template<int>
    class EntityPointer;
    class CpGridData;

        /// @brief
        /// @todo Doc me!
        /// @tparam
        class Intersection
        {
        public:
            /// @brief
            /// @todo Doc me!
            enum { dimension = 3 };
            enum { dimensionworld = 3 };
            /// @brief
            /// @todo Doc me!
            typedef cpgrid::Entity<0> Entity;
            typedef cpgrid::EntityPointer<0> EntityPointer;
             typedef cpgrid::Geometry<2,3> Geometry;
             typedef cpgrid::Geometry<2,3> LocalGeometry;
            typedef double ctype;
            typedef FieldVector<ctype, 2> LocalCoordinate;
            typedef FieldVector<ctype, 3> GlobalCoordinate;

            /// @brief
            /// @todo Doc me!
            /// @param
            Intersection()
                : pgrid_(0),
                  index_(-1),
                  subindex_(-1),
                  faces_of_cell_(),
                  global_geom_(),
//                   in_inside_geom_(),
                  nbcell_(-1), // Init to self, which is invalid.
                  is_on_boundary_(false)
            {
            }
            /// @brief
            /// @todo Doc me!
            /// @param
            Intersection(const CpGridData& grid, const EntityRep<0>& cell, int subindex, bool update_now = true);

            /// @brief
            /// @todo Doc me!
            /// @param
            /// @return
            bool operator==(const Intersection& other) const
            {
                return subindex_ == other.subindex_  &&  index_ == other.index_  &&  pgrid_ == other.pgrid_;
            }

            /// @brief
            /// @todo Doc me!
            /// @param
            /// @return
            bool operator!=(const Intersection& other) const
            {
                return !operator==(other);
            }

            /// @brief
            /// @todo Doc me!
            /// @param
            /// @return
            bool boundary() const
            {
                return is_on_boundary_;
            }

            /// Returns the boundary id of this intersection.
            int boundaryId() const;

            /// Returns the boundary segment index of this intersection.
            int boundarySegmentIndex() const;

            /// @brief
            /// @todo Doc me!
            /// @return
            bool neighbor() const
            {
                return !boundary() && nbcell_!=std::numeric_limits<int>::max();
            }

            /// @brief
            /// @todo Doc me!
            /// @return
            EntityPointer inside() const;

            /// @brief
            /// @todo Doc me!
            /// @return
            EntityPointer outside() const;

            /// @brief
            /// @todo Doc me!
            /// @return
            bool conforming() const
            {
                return boundary(); // I.e. we are assuming all nonconforming interior.
            }

            // Geometrical information about this intersection in
            // local coordinates of the inside() entity.
            /// @brief
            /// @todo Doc me!
            /// @return
            const LocalGeometry& geometryInInside() const
            {
                EWOMS_THROW(std::runtime_error, "This intersection class does not support geometryInInside().");
//                 return in_inside_geom_;
            }

            // Geometrical information about this intersection in
            // local coordinates of the outside() entity.
            /// @brief
            /// @todo Doc me!
            /// @return
            const LocalGeometry& geometryInOutside() const
            {
                if (boundary()) {
                    EWOMS_THROW(std::runtime_error, "Cannot access geometryInOutside(), intersection is at a boundary.");
                }
                EWOMS_THROW(std::runtime_error, "This intersection class does not support geometryInOutside().");
//                 return in_outside_geom_;
            }

            /// @brief
            /// @todo Doc me!
            /// @return
            const Geometry& geometry() const
            {
                return global_geom_;
            }

            /// @brief
            /// @todo Doc me!
            /// @return
            GeometryType type() const
            {
                return geometry().type();
            }

            /// Local index of codim 1 entity in the inside() entity
            /// where intersection is contained in.
            int indexInInside() const;

            /// Local index of codim 1 entity in outside() entity
            /// where intersection is contained in.
            int indexInOutside() const
            {
                int in_inside = indexInInside();
                if (in_inside == -1) {
                    // NNC face, return -1 here as well.
                    return -1;
                }
                return in_inside + ((in_inside % 2) ? -1 : 1);
            }

            /// @brief
            /// @todo Doc me!
            /// @param
            /// @return
            FieldVector<ctype, 3> outerNormal(const FieldVector<ctype, 2>&) const;

            /// @brief
            /// @todo Doc me!
            /// @param
            /// @return
            FieldVector<ctype, 3> integrationOuterNormal(const FieldVector<ctype, 2>& unused) const;

            /// @brief
            /// @todo Doc me!
            /// @param
            /// @return
            FieldVector<ctype, 3> unitOuterNormal(const FieldVector<ctype, 2>&) const;

            /// @brief
            /// @todo Doc me!
            /// @param
            /// @return
            FieldVector<ctype, 3> centerUnitOuterNormal() const;

            int id() const
            {
                const EntityRep<1>& face = faces_of_cell_[subindex_];
                return face.index();
            }

        protected:
            const CpGridData* pgrid_;
            int index_;
            int subindex_;
            OrientedEntityTable<0,1>::row_type faces_of_cell_;
            Geometry global_geom_;
//             LocalGeometry in_inside_geom_;
//             LocalGeometry in_outside_geom_;
            int nbcell_;
            bool is_on_boundary_;

            void increment();

            void update();

            void setAtEnd()
            {
                subindex_ = faces_of_cell_.size();
            }

            bool isAtEnd() const
            {
                return subindex_ == faces_of_cell_.size();
            }

            int nbcell() const
            {
                if (is_on_boundary_) {
                    EWOMS_THROW(std::runtime_error, "There is no outside cell, intersection is at boundary.");
                }
                if(nbcell_==std::numeric_limits<int>::max())
                    EWOMS_THROW(std::runtime_error, "There is no outside cell, intersection is at processor boundary.");
                return nbcell_;
            }
        };

        class IntersectionIterator : public Intersection
        {
        public:
            typedef cpgrid::Intersection Intersection;

            IntersectionIterator()
                : Intersection()
            {
            }

            IntersectionIterator(const CpGridData& grid, const EntityRep<0>& cell, bool at_end)
                : Intersection(grid, cell, 0, !at_end)
            {
                if (at_end) {
                    Intersection::setAtEnd();
                } else {
                    Intersection::update();
                }
            }

            IntersectionIterator& operator++()
            {
                Intersection::increment();
                return *this;
            }

            const Intersection* operator->() const
            {
                assert(!Intersection::isAtEnd());
                return this;
            }

            const Intersection& operator*() const
            {
                assert(!Intersection::isAtEnd());
                return *this;
            }

        };

    } // namespace cpgrid
} // namespace Dune

#endif // EWOMS_INTERSECTION_HEADER
