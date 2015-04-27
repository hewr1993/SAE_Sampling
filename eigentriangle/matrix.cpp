#include "matrix.h"

// matrix product a vector
vector<double>  Matrix::Product(vector<double> B)
{
    vector<double> res;
    if ((int) B.size() != this -> GetColNum())
        return res;
    for (int a = 0; a < this -> GetRowNum(); a ++)
    {
        double val = 0;
        for (int b = 0; b < this -> GetColNum(); b ++)
            val += matrix[a][b] * B[b];
        res.push_back(val);
    }
    return res;
}

// vector product a matrix
vector<double>  Matrix::VProduct(vector<double> B)
{
    vector<double> res;
    if ((int) B.size() != this -> GetRowNum())
        return res;
    for (int a = 0; a < this -> GetColNum(); a ++)
    {
        double val = 0;
        for (int b = 0; b < this -> GetRowNum(); b ++)
            val += matrix[b][a] * B[b];
        res.push_back(val);
    }
    return res;
}

// matrix production 
Matrix*         Matrix::Product(Matrix* B)
{
    if (this -> GetColNum() != B -> GetRowNum())
        return NULL;
    Matrix* C = new Matrix();
    C -> Create(this -> GetRowNum(), B -> GetColNum());
    for (int a = 0; a < this -> GetRowNum(); a ++)
    {
        for (int b = 0; b < B -> GetColNum(); b ++)
        {
            double val = 0;
            for (int c = 0; c < this -> GetColNum(); c ++)
                val += this -> Get(a, c) * B -> Get(c, b);
            C -> Set(a, b, val);
        }
    }
    return C;
}

double      Matrix::Lanczos(int k)
{
    // TBD
    return 0;
}

