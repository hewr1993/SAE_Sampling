#include <Eigen/Sparse>
#include "storage/mgraph.h"
#include "solver/solver.h"


using Eigen::RowMajor;
using Eigen::SparseMatrix;

typedef SparseMatrix<double, RowMajor> SparseMatrixType;

class EigenTriangle : public sae::Solver<int>
{
public:
    EigenTriangle(sae::io::MappedGraph *graph);
    ~EigenTriangle();
    // return approximate number of triangles
    double solve(int maxIter, double tol);
};

