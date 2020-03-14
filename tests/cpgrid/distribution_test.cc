#include <config.h>

#include <dune/common/version.hh>

#define BOOST_TEST_MODULE DistributedCpGridTests
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include <ewoms/eclgrids/cpgrid.hh>

// Warning suppression for Dune includes.

#include <dune/geometry/referenceelements.hh>
#include <dune/common/fvector.hh>
#include <dune/common/parallel/variablesizecommunicator.hh>
#if HAVE_DUNE_GRID_CHECKS
// The header below are not installed for dune-grid
// Therefore we need to deactivate testing, if they
// not available
#include <dune/grid/test/checkpartition.hh>
#include <dune/grid/test/checkcommunicate.hh>

#endif

namespace Dune
{
template<int dim>
Dune::cpgrid::Entity<dim> createEntity(const Dune::CpGrid& grid, int index, bool orientation)
{
    return Dune::cpgrid::Entity<dim>(*grid.current_view_data_, index, orientation);
}
/*
template Dune::cpgrid::Entity<1> createEntity<1>(const Dune::CpGrid& grid, int index, bool orientation);
template Dune::cpgrid::Entity<3> createEntity<3>(const Dune::CpGrid& grid, int index, bool orientation);
*/
} // end namespace Dune

#if HAVE_MPI
class MPIError {
public:
  /** @brief Constructor. */
  MPIError(std::string s, int e) : errorstring(s), errorcode(e){}
  /** @brief The error string. */
  std::string errorstring;
  /** @brief The mpi error code. */
  int errorcode;
};

void mpiErrorHandler(MPI_Comm *, int *errCode, ...){
  char *errString=new char[MPI_MAX_ERROR_STRING];
  int errLength;
  MPI_Error_string(*errCode, errString, &errLength);
  std::string s(errString, errLength);
  std::cerr << "An MPI Error ocurred:"<<std::endl<<s<<std::endl;
  delete[] errString;
  throw MPIError(s, *errCode);
}
#endif

class LoadBalanceGlobalIdDataHandle
{
public:
    LoadBalanceGlobalIdDataHandle(const Dune::CpGrid::GlobalIdSet& gid_set,
                                  const Dune::CpGrid& grid,
                                  std::vector<int>& dist_point_ids,
                                  std::vector<int>& dist_cell_ids)
        : gid_set_(gid_set), grid_(grid), dist_point_ids_(dist_point_ids),
          dist_cell_ids_(dist_cell_ids)
    {}
    typedef int DataType;
    bool fixedsize(int /*dim*/, int /*codim*/)
    {
        return true;
    }

    template<class T>
    std::size_t size(const T&)
    {
        return 1;
    }
    template<class B, class T>
    void gather(B& buffer, const T& t)
    {
        buffer.write(gid_set_.id(t));

    }
    template<class B, class T>
    void scatter(B& buffer, const T& t, std::size_t)
    {
        int gid;
        buffer.read(gid);
        if(T::codimension==3)
            dist_point_ids_[grid_.leafIndexSet().index(t)]=gid;
        if(T::codimension==0)
            dist_cell_ids_[grid_.leafIndexSet().index(t)]=gid;
    }
    bool contains(int dim, int codim)
    {
        return dim==3 && (codim<=1 || codim==3);
    }
private:
    const Dune::CpGrid::GlobalIdSet& gid_set_;
    const Dune::CpGrid& grid_;
    std::vector<int>& dist_point_ids_;
    std::vector<int>& dist_cell_ids_;
};

/// \brief A data handle to use with CpGrid::.cellScatterGatherInterface()
/// that checks the correctness of the global cell index at the receiving
/// end.
class CheckGlobalCellHandle
{
public:
    CheckGlobalCellHandle(const std::vector<int>& sendindex,
                          const std::vector<int>& recvindex)
        : sendindex_(sendindex), recvindex_(recvindex)
    {}

    typedef int DataType;
    bool fixedsize()
    {
        return true;
    }

    template<class T>
    std::size_t size(const T&)
    {
        return 1;
    }
    template<class B>
    void gather(B& buffer, std::size_t i)
    {
        buffer.write(sendindex_[i]);
    }
    template<class B>
    void scatter(B& buffer, const std::size_t& i, std::size_t)
    {
        int gid;
        buffer.read(gid);
        BOOST_REQUIRE(gid==recvindex_[i]);
    }
private:
    const std::vector<int>& sendindex_;
    const std::vector<int>& recvindex_;
};

class GatherGlobalIdDataHandle
{
public:
    GatherGlobalIdDataHandle(const Dune::CpGrid::GlobalIdSet& gathered_gid_set,
                             const Dune::CpGrid::LeafIndexSet& distributed_indexset,
                       std::vector<int>& dist_point_ids,
                       std::vector<int>& dist_cell_ids)
        : gathered_gid_set_(gathered_gid_set), distributed_indexset_(distributed_indexset),
          dist_point_ids_(dist_point_ids),
          dist_cell_ids_(dist_cell_ids)
    {}

    typedef int DataType;
    bool fixedsize(int /*dim*/, int /*codim*/)
    {
        return true;
    }

    template<class T>
    std::size_t size(const T&)
    {
        return 1;
    }
    template<class B, class T>
    void gather(B& buffer, const T& t)
    {
        if(T::codimension==0)
            buffer.write(dist_cell_ids_[distributed_indexset_.index(t)]);
        if(T::codimension==3)
            buffer.write(dist_point_ids_[distributed_indexset_.index(t)]);
    }
    template<class B, class T>
    void scatter(B& buffer, const T& t, std::size_t)
    {
        int gid;
        buffer.read(gid);
        if(gid!=gathered_gid_set_.id(t))
            EWOMS_THROW(std::runtime_error, "Exspected a different global id");
    }
    bool contains(int dim, int codim)
    {
        return dim==3 && (codim<=1 || codim==3);
    }
private:
    const Dune::CpGrid::GlobalIdSet& gathered_gid_set_;
    const Dune::CpGrid::LeafIndexSet& distributed_indexset_;
    std::vector<int>& dist_point_ids_;
    std::vector<int>& dist_cell_ids_;
};

/// \brief A data handle to use with CpGrid::cellScatterGatherInterface()
/// that checks the correctness of the unique boundary ids at the receiving
/// end.
///
/// We used fixedsize for the message as there is a bug in DUNE up to 2.6.0
class CheckBoundaryIdHandle
{
public:
    CheckBoundaryIdHandle(const Dune::CpGrid& sendGrid,
                          const Dune::CpGrid& recvGrid)
        : sendGrid_(sendGrid),
          recvGrid_(recvGrid)
    {}

    typedef int DataType;
    bool fixedsize()
    {
        // We used fixedsize for the message as there is a bug in DUNE
        // up to 2.6.0
        return true;
        //return false;
    }

    template<class T>
    std::size_t size(const T& )
    {
        return 6;
        //return sendGrid_.numCellFaces(i);
    }
    template<class B>
    void gather(B& buffer, std::size_t i)
    {
        auto nofaces = sendGrid_.numCellFaces(i);
        int j=0;
        for(; j < nofaces; ++j)
        {
            buffer.write(sendGrid_.boundaryId(sendGrid_.cellFace(i, j)));
        }
        // We used fixedsize for the message as there is a bug in DUNE
        // up to 2.6.0. Fill the buffer with bogus numbers
        for(; j < 6; ++j)
            buffer.write(-1);
    }
    template<class B>
    void scatter(B& buffer, const std::size_t& i, std::size_t n)
    {
        BOOST_REQUIRE(static_cast<int>(n) == recvGrid_.numCellFaces(i));
        std::size_t j = 0;
        int id;
        for(; j < n; ++j)
        {
            buffer.read(id);
            BOOST_REQUIRE(id == recvGrid_.boundaryId(recvGrid_.cellFace(i, j)));
        }
        // We used fixedsize for the message as there is a bug in DUNE
        // up to 2.6.0. read the bogus numbers from buffer.
        for(;j<6; ++j)
            buffer.read(id);
    }
private:
    const Dune::CpGrid& sendGrid_;
    const Dune::CpGrid& recvGrid_;
};

class DummyDataHandle
{
public:
    typedef double DataType;
    bool fixedsize(int /*dim*/, int /*codim*/)
    {
        return true;
    }

    template<class T>
    std::size_t size(const T&)
    {
        return 1;
    }
    template<class B, class T>
    void gather(B& buffer, const T&)
    {
        buffer.write(100.0);
    }
    template<class B, class T>
    void scatter(B& buffer, const T&, std::size_t s)
    {
        double val;
        //std::cout<<"Scattering ";
        for(std::size_t i=0; i<s; ++i)
        {
            buffer.read(val);
            //  std::cout<<val<<" "<<i<<" ";
        }
        //std::cout<<"to "<<t.index()<<" with codim"<<T::codimension<<std::endl;
    }
    bool contains(int dim, int codim)
    {
        return dim==3 && (codim<=1 || codim==3);
    }
};

class CopyCellValues
{
public:
    CopyCellValues(std::vector<int>& cont)
        : cont_(cont)
    {}

    typedef int DataType;
    bool fixedsize(int /*dim*/, int /*codim*/)
    {
        return true;
    }

    template<class T>
    std::size_t size(const T&)
    {
        return 1;
    }
    template<class B, class T>
    void gather(B& buffer, const T& t)
    {
        buffer.write(cont_[t.index()]);
    }
    template<class B, class T>
    void scatter(B& buffer, const T& t, std::size_t s)
    {
        for(std::size_t i=0; i<s; ++i)
        {
            buffer.read(cont_[t.index()]);
        }
    }
    bool contains(int dim, int codim)
    {
        return dim==3 && codim==0;
    }
private:
    std::vector<int>& cont_;
};

BOOST_AUTO_TEST_CASE(testDistributedComm)
{
#if HAVE_MPI
    Dune::CpGrid grid;
    std::array<int, 3> dims={{8, 4, 2}};
    std::array<double, 3> size={{ 8.0, 4.0, 2.0}};
    //grid.setUniqueBoundaryIds(true); // set and compute unique boundary ids.
    grid.createCartesian(dims, size);
    grid.loadBalance();
#ifdef HAVE_DUNE_ISTL
    using AttributeSet = Dune::OwnerOverlapCopyAttributeSet::AttributeSet;
#else
    /// \brief The type of the set of the attributes
    enum AttributeSet{owner, overlap, copy};
#endif
    std::vector<int> cont(grid.size(0), 1);
    const auto& indexSet = grid.getCellIndexSet();
    for ( const auto index: indexSet)
        if (index.local().attribute() != AttributeSet::owner )
            cont[index.local()] = -1;

    CopyCellValues handle(cont);
    grid.communicate(handle, Dune::InteriorBorder_All_Interface,
                     Dune::ForwardCommunication);

    for ( const auto index: indexSet)
        BOOST_REQUIRE(cont[index.local()] == 1);
#endif
}

BOOST_AUTO_TEST_CASE(compareWithSequential)
{
#if HAVE_MPI
    Dune::CpGrid grid;
    Dune::CpGrid seqGrid(MPI_COMM_SELF);
    std::array<int, 3> dims={{8, 4, 2}};
    std::array<double, 3> size={{ 8.0, 4.0, 2.0}};
    grid.setUniqueBoundaryIds(true); // set and compute unique boundary ids.
    seqGrid.setUniqueBoundaryIds(true);
    grid.createCartesian(dims, size);
    grid.loadBalance();
    seqGrid.createCartesian(dims, size);

    auto idSet = grid.globalIdSet(), seqIdSet = seqGrid.globalIdSet();

    using GridView = Dune::CpGrid::LeafGridView;
    using ElementIterator = GridView::Codim<0>::Iterator;
    GridView gridView(grid.leafGridView());
    GridView seqGridView(seqGrid.leafGridView());

    ElementIterator endEIt = gridView.end<0>();
    ElementIterator seqEndEIt = seqGridView.end<0>();
    ElementIterator seqEIt = seqGridView.begin<0>();
    const auto& gc = grid.globalCell();
    const auto& seqGc = seqGrid.globalCell();
    int i{}, seqI{};
    BOOST_REQUIRE(gc.size() == std::size_t(grid.size(0)));

    for (ElementIterator eIt = gridView.begin<0>(); eIt != endEIt; ++eIt, ++i) {
        // find corresponding cell in global grid
        auto id = idSet.id(*eIt);
        while (seqIdSet.id(*seqEIt) < id && seqEIt != seqEndEIt)
        {
            ++seqI;
            ++seqEIt;
        }
        BOOST_REQUIRE(id == seqIdSet.id(seqEIt));
        BOOST_REQUIRE(gc[eIt->index()] == seqGc[seqEIt->index()]);
        const auto& geom = eIt->geometry();
        const auto& seqGeom = seqEIt-> geometry();
        BOOST_REQUIRE(geom.center() == seqGeom.center());
        BOOST_REQUIRE(geom.volume() == seqGeom.volume());

        int ii{};

        for (auto iit=gridView.ibegin(*eIt), siit = seqGridView.ibegin(*seqEIt),
                 endiit = gridView.iend(*eIt); iit!=endiit; ++iit, ++siit, ++ii)
            {
                if (iit.boundary())
                {
                    BOOST_REQUIRE(iit.boundarySegmentIndex() == siit.boundarySegmentIndex());
                    BOOST_REQUIRE(iit.boundaryId() == siit.boundaryId());
                }
                BOOST_REQUIRE(iit->geometry().center() == siit->geometry().center());
                BOOST_REQUIRE(iit->geometry().volume() == siit->geometry().volume());
                BOOST_REQUIRE(iit.boundary() == siit.boundary());
                BOOST_REQUIRE(iit.outerNormal({0, 0}) == siit.outerNormal({0, 0}));
                BOOST_REQUIRE(idSet.id(*iit.inside()) == seqIdSet.id(*siit.inside()));
                if (iit->neighbor())
                {
                    assert(siit->neighbor());
                    BOOST_REQUIRE(idSet.id(*iit.outside()) == seqIdSet.id(*siit.outside()));
                }
            }

        // to reach all points we need to loop over subentities
        int faces = grid.numCellFaces(eIt->index());
        BOOST_REQUIRE(faces == seqGrid.numCellFaces(seqEIt.index()));
        for (int f = 0; f < faces; ++f)
        {
            using namespace Dune::cpgrid;
            auto face = grid.cellFace(eIt->index(), f);
            auto seqFace = seqGrid.cellFace(seqEIt->index(), f);
            BOOST_REQUIRE(idSet.id(Dune::createEntity<1>(grid, face, true)) ==
                          seqIdSet.id(Dune::createEntity<1>(seqGrid, seqFace, true)));
            int vertices = grid.numFaceVertices(face);
            BOOST_REQUIRE(vertices == seqGrid.numFaceVertices(seqFace));
            for (int v = 0; v < vertices; ++v)
            {
                auto vertex = grid.faceVertex(face, v);
                auto seqVertex = seqGrid.faceVertex(seqFace, v);
                BOOST_REQUIRE(idSet.id(Dune::createEntity<3>(grid, vertex, true)) ==
                              seqIdSet.id(Dune::createEntity<3>(seqGrid, seqVertex, true)));
                BOOST_REQUIRE(grid.vertexPosition(vertex) ==
                              seqGrid.vertexPosition(seqVertex));
            }
        }
    }
#endif
}

BOOST_AUTO_TEST_CASE(distribute)
{

    int m_argc = boost::unit_test::framework::master_test_suite().argc;
    char** m_argv = boost::unit_test::framework::master_test_suite().argv;
    Dune::MPIHelper::instance(m_argc, m_argv);
    int procs=1;
#if HAVE_MPI
    MPI_Errhandler handler;
    MPI_Comm_create_errhandler(mpiErrorHandler, &handler);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, handler);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
#endif
    Dune::CpGrid grid;
    std::array<int, 3> dims={{10, 10, 10}};
    std::array<double, 3> size={{ 1.0, 1.0, 1.0}};

    grid.createCartesian(dims, size);
#if HAVE_MPI
    BOOST_REQUIRE(grid.comm()==MPI_COMM_WORLD);
#endif
    std::vector<int> cell_indices, face_indices, point_indices;
    std::vector<Dune::CpGrid::Traits::Codim<0>::Geometry::GlobalCoordinate > cell_centers, face_centers, point_centers;

    typedef Dune::CpGrid::LeafGridView GridView ;
    GridView gridView = grid.leafGridView();

    int cell_size = gridView.size(0);
    int face_size = gridView.size(1);
    int point_size = gridView.size(3);

    typedef GridView :: IndexSet IndexSet;
    const IndexSet& ix = gridView.indexSet();

    if(procs==1)
    {
        typedef GridView :: Codim<0> :: Iterator LeafIterator ;
        for (LeafIterator it = gridView.begin<0>();
             it != gridView.end<0>(); ++it) {
            auto ref = Dune::ReferenceElements<Dune::CpGrid::ctype,3>::cube();

            cell_indices.push_back(ix.index(*it));
            cell_centers.push_back(it->geometry().center());
            typedef GridView :: IntersectionIterator IntersectionIterator;
            for(IntersectionIterator iit=gridView.ibegin(*it),
                    endiit = gridView.iend(*it); iit!=endiit; ++iit)
            {
                //            face_indices.push_back(ix.index(*it->subEntity<1>(iit->indexInInside())));
                face_centers.push_back(iit->geometry().center());
                for(int i=0; i<4; ++i){
                    point_indices.push_back(ix.subIndex(*it, ref.subEntity(iit->indexInInside(),1,i,3), 3));
                    //ref.subEntity(iit->indexInInside(),1,i,dim).geometry().center();
                }
            }
        }
    }
    DummyDataHandle data;

    const Dune::CpGrid::GlobalIdSet& unbalanced_gid_set=grid.globalIdSet();

    grid.communicate(data, Dune::All_All_Interface, Dune::ForwardCommunication);
    grid.loadBalance(data);

    if ( grid.numCells())
    {
        std::array<int,3> ijk;
        grid.getIJK(0, ijk);
    }

#if HAVE_MPI
    // Dune::CpGrid::loadBalance() is non-trivial only if we have MPI
    // *and* if the target Dune platform is sufficiently recent.
    BOOST_REQUIRE(grid.comm()!=MPI_COMM_SELF||MPI_COMM_WORLD==MPI_COMM_SELF);
#endif // HAVE_MPI

    if(procs==1)
    {
        // Check whether the scattered grid is identical to the orinal one.
        BOOST_REQUIRE(cell_size  == gridView.size(0));
        BOOST_REQUIRE(face_size  == gridView.size(1));
        BOOST_REQUIRE(point_size == gridView.size(3));

        int cell_index=0, face_index=0, point_index=0;

        const Dune::CpGrid::LeafIndexSet& ix1 = grid.leafIndexSet();
#if HAVE_MPI
        BOOST_REQUIRE(&ix!=&ix1);
#endif

        for (Dune::CpGrid::Codim<0>::LeafIterator it = grid.leafbegin<0>();
             it != grid.leafend<0>(); ++it) {
            auto ref = Dune::ReferenceElements<Dune::CpGrid::ctype,3>::cube();

            BOOST_REQUIRE(cell_indices[cell_index]==ix1.index(*it));
            BOOST_REQUIRE(cell_centers[cell_index++]==it->geometry().center());
            for(Dune::CpGrid::LeafIntersectionIterator iit=gridView.ibegin(*it),
                    endiit = gridView.iend(*it); iit!=endiit; ++iit)
            {
                //BOOST_REQUIRE(face_indices[face_index]==ix1.index(*it->subEntity<1>(iit->indexInInside())));
                BOOST_REQUIRE(face_centers[face_index++]==iit->geometry().center());
                for(int i=0; i<4; ++i){
                    BOOST_REQUIRE(point_indices[point_index++]==ix1.subIndex(*it, ref.subEntity(iit->indexInInside(),1,i,3), 3));
                    //ref.subEntity(iit->indexInInside(),1,i,dim).geometry().center();
                }
            }
        }
    }else
    {
#if HAVE_DUNE_GRID_CHECKS
        //checkCommunication(grid,-1,Dune::dvverb); // Deactivated as one has to patch cpgrid to support Intersection::geometryInInside and Outside
        checkPartitionType( gridView );
#endif
        std::vector<int> point_ids(grid.leafIndexSet().size(3)), cell_ids(grid.leafIndexSet().size(0));
        LoadBalanceGlobalIdDataHandle lb_gid_data(unbalanced_gid_set,
                                                  grid,
                                                  point_ids,
                                                  cell_ids);
        grid.scatterData(lb_gid_data);
        GatherGlobalIdDataHandle gather_gid_set_data(unbalanced_gid_set,
                                                     grid.leafIndexSet(),
                                                     point_ids,
                                                     cell_ids);
        grid.gatherData(gather_gid_set_data);

    }
}

// A small test that gathers/scatter the global cell indices.
// On the sending side these are sent and on the receiving side
// these are check with the globalCell values.
BOOST_AUTO_TEST_CASE(cellGatherScatterWithMPI)
{

    Dune::CpGrid grid;
    std::array<int, 3> dims={{8, 4, 2}};
    std::array<double, 3> size={{ 8.0, 4.0, 2.0}};
    grid.createCartesian(dims, size);
    typedef Dune::CpGrid::LeafGridView GridView;
    enum{dimWorld = GridView::dimensionworld};

    grid.loadBalance();
    auto global_grid = grid;
    global_grid.switchToGlobalView();

    auto scatter_handle = CheckGlobalCellHandle(global_grid.globalCell(),
                                                grid.globalCell());
    auto gather_handle  = CheckGlobalCellHandle(grid.globalCell(),
                                                global_grid.globalCell());
    auto bid_handle     = CheckBoundaryIdHandle(global_grid, grid);

#if HAVE_MPI
    Dune::VariableSizeCommunicator<> scatter_gather_comm(grid.comm(), grid.cellScatterGatherInterface(), 8*4*2*8);
    scatter_gather_comm.forward(scatter_handle);
    scatter_gather_comm.backward(gather_handle);
    scatter_gather_comm.forward(bid_handle);
#else
    (void) scatter_handle;
    (void) gather_handle;
    (void) bid_handle;
#endif
}

BOOST_AUTO_TEST_CASE(intersectionOverlap)
{
    Dune::CpGrid grid;
    std::array<int, 3> dims={{8, 4, 2}};
    std::array<double, 3> size={{ 8.0, 4.0, 2.0}};
    grid.createCartesian(dims, size);
    grid.setUniqueBoundaryIds(true); // set and compute unique boundary ids.
    typedef Dune::CpGrid::LeafGridView GridView;
    GridView gridView(grid.leafGridView());
    enum{dimWorld = GridView::dimensionworld};
    typedef typename GridView::ctype CoordScalar;
    typedef Dune::FieldVector<CoordScalar,dimWorld> GlobalPosition;
    typedef GridView::Codim<0>::Iterator ElementIterator;
    typedef typename GridView::IntersectionIterator IntersectionIterator;

    grid.loadBalance();
    ElementIterator endEIt = gridView.end<0>();
    for (ElementIterator eIt = gridView.begin<0>(); eIt != endEIt; ++eIt) {
        IntersectionIterator isEndIt = gridView.iend(eIt);
        for (IntersectionIterator isIt = gridView.ibegin(eIt); isIt != isEndIt; ++isIt)
        {
            if (isIt->neighbor())
            {
                GlobalPosition distVec = eIt->geometry().center() -
                    isIt->outside()->geometry().center();
                // Make sure that Coordinates of an element and its neighbor are not identical
                BOOST_REQUIRE(distVec.two_norm2()>=1e-8);
            }
        }
    }
}

bool
init_unit_test_func()
{
    return true;
}

int main(int argc, char** argv)
{
    Dune::MPIHelper::instance(argc, argv);

#if defined(HAVE_MPI) && HAVE_MPI
    MPI_Errhandler errhandler;
    MPI_Comm_create_errhandler(mpiErrorHandler, &errhandler);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, errhandler);
#endif // HAVE_MPI

    boost::unit_test::unit_test_main(&init_unit_test_func,
                                     argc, argv);
}
