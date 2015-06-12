#include <Eigen/Sparse>
#include "storage/mgraph.h"
#include "solver/solver.h"
#include <vector>

using Eigen::RowMajor;
using Eigen::SparseMatrix;
using std::vector;

typedef SparseMatrix<double, RowMajor> SparseMatrixType;

class EigenTriangle : public sae::Solver<int>
{
public:
    EigenTriangle(sae::io::MappedGraph *graph);
    ~EigenTriangle();
    // return approximate number of triangles
    double solve(int maxIter, double tol);
    double solve(int maxIter);
    double solve(double k);
private:
    SparseMatrix<double> adjMatrix;
    SparseMatrix<double, RowMajor> omega; // used for partial reorthogonalization
private:
    double* vals;
    int*    rows;
    int*    cols;
    vector<std::pair<int, int> > edgeList;
};

