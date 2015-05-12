#include <Eigen/Sparse>

using Eigen::RowMajor;
using Eigen::SparseMatrix;

typedef SparseMatrix<double, RowMajor> SparseMatrixType;

class EigenTriangle 
{
public:
    EigenTriangle();
    ~EigenTriangle();
    // return approximate number of triangles
    double                  count(SparseMatrixType adjMatrix, int maxIter, double tol);
	double                  count(SparseMatrixType adjMatrix, int maxIter);    
    // temporal: build adjacent matrix (todo: integrate to SAE)
    SparseMatrixType        init(const char* fileDir);     
};

