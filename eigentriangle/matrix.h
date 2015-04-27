#pragma once

#include <vector>
#include <cmath>
#include <cstdio>
#include <string>

using std::vector;
using std::string;

class       Matrix
{
private:
    int         rowNum;
    int         colNum;
    double**    matrix;
public:
    int         GetRowNum()
    {
        return rowNum;
    }
    int         GetColNum()
    {
        return colNum;
    }
    // get matrix[row][col]
    double      Get(int row, int col)
    {
        if (row >= rowNum || col >= colNum)
            return -1;
        return matrix[row][col];
    }

    // set matrix[row][col] = val
    int         Set(int row, int col, double val)
    {
        if (row >= rowNum || col >= colNum)
            return -1;
        matrix[row][col] = val;
        return 0;
    }

    // create a matrix with rowNum * colNum
    int        Create(int rowNum, int colNum)
    {
        this -> rowNum = rowNum;
        this -> colNum = colNum;
        matrix = new double*[rowNum];
        for (int i = 0; i < rowNum; i ++)
            matrix[i] = new double[colNum];
        return 0;
    }

    int         Print()
    {
        for (int i = 0; i < rowNum; i ++)
        {
            for (int j = 0; j < colNum; j ++)
                printf("%.3lf ", matrix[i][j]);
            printf("\n");
        }
        return 0;
    }

    int         Print(string fileDir)
    {
        FILE* fout = fopen(fileDir.c_str(), "w");
        for (int i = 0; i < rowNum; i ++)
        {
            for (int j = 0; j < colNum; j ++)
                fprintf(fout, "%.5lf ", matrix[i][j]);
            fprintf(fout, "\n");
        }
        return 0;
    }

    // matrix product a vector
    vector<double>  Product(vector<double> B);

    // vector product a matrix
    vector<double>  VProduct(vector<double> B);

    // matrix production 
    Matrix*     Product(Matrix* B);

    // Lanczos method for calculating the k-th eigenvalue
    double      Lanczos(int k);
};

