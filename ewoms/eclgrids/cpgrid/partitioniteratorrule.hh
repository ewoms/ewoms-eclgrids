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
namespace Dune
{
namespace cpgrid
{
    /// A rule at what entities to stop
    ///
    template<PartitionIteratorType pitype>
    struct PartitionIteratorRule
    {
        enum {fullSet=false, emptySet=true};

        template<int codim>
        bool isInvalid(const Entity<codim>&)
        {
            return true;
        }
    };

    template<>
    struct PartitionIteratorRule<Interior_Partition>
    {
        enum {fullSet=false, emptySet=false};
        template<int codim>
        bool isInvalid(const Entity<codim>& e)
        {
            if(e.partitionType()==InteriorEntity)
                return false;
            return true;
        }
    };

    template<>
    struct PartitionIteratorRule<InteriorBorder_Partition>
    {
        enum {fullSet=false, emptySet=false};
        template<int codim>
        bool isInvalid(const Entity<codim>& e)
        {
            if(e.partitionType()==InteriorEntity ||
               e.partitionType()==BorderEntity)
                return false;
            return true;
        }
    };

    template<>
    struct PartitionIteratorRule<Overlap_Partition>
    {
        enum {fullSet=false, emptySet=false};
        template<int codim>
        bool isInvalid(const Entity<codim>& e)
        {
            // interior, border, and overlap are valid!
            if(e.partitionType()==FrontEntity)
                return true;
            return false;
        }
    };

    template<>
    struct PartitionIteratorRule<All_Partition>
    {
        enum {fullSet=true, emptySet=false};
        template<int codim>
        bool isInvalid(const Entity<codim>&)
        {
            return false;
        }
    };

    template<>
    struct PartitionIteratorRule<OverlapFront_Partition>
        : public PartitionIteratorRule<All_Partition>
    {
        // Visits everything but ghost entities.
        // As there are no ghosts in CpGrid, it visits
        // everything.
    };

} // end namespace cpgrid
} // end namespace Dune
