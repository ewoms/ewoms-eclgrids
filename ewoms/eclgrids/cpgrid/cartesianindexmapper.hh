#ifndef EWOMS_CPGRIDCARTESIANINDEXMAPPER_HEADER
#define EWOMS_CPGRIDCARTESIANINDEXMAPPER_HEADER

#include <array>
#include <cassert>

#include <ewoms/eclgrids/common/cartesianindexmapper.hh>
#include <ewoms/eclgrids/cpgrid.hh>

namespace Dune
{
    template<>
    class CartesianIndexMapper< CpGrid >
    {
    public:
        static const int dimension = 3 ;
    protected:
        typedef CpGrid Grid;
        const Grid& grid_;
        const int cartesianSize_;

        int computeCartesianSize() const
        {
            int size = cartesianDimensions()[ 0 ];
            for( int d=1; d<dimension; ++d )
                size *= cartesianDimensions()[ d ];
            return size ;
        }

    public:
        explicit CartesianIndexMapper( const Grid& grid )
            : grid_( grid ),
              cartesianSize_( computeCartesianSize() )
        {
        }

        const std::array<int, dimension>& cartesianDimensions() const
        {
            return grid_.logicalCartesianSize();
        }

        int cartesianSize() const
        {
            return cartesianSize_;
        }

        int compressedSize() const
        {
            return grid_.globalCell().size();
        }

        int cartesianIndex( const int compressedElementIndex ) const
        {
            assert(  compressedElementIndex >= 0 && compressedElementIndex < compressedSize() );
            return grid_.globalCell()[ compressedElementIndex ];
        }

        void cartesianCoordinate(const int compressedElementIndex, std::array<int,dimension>& coords) const
        {
            grid_.getIJK( compressedElementIndex, coords );
        }
    };

} // end namespace Ewoms
#endif
