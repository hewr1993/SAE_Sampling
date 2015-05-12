#include "eigen_triangle.h"
#include <vector>
#include <cmath>
#include <sstream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <complex>
#include <Eigen/Dense>
#include <Eigen/Sparse>

using std::vector;
using std::string;
using std::istringstream;
using Eigen::VectorXd;
using Eigen::MatrixXd;

vector<string>     ReadFromFile(const char* fileDir)
{
    printf("reading from %s\n", fileDir);
    char        buf[655360];
    char*       eof;
    string      line;

    FILE        *fin = fopen(fileDir, "r");
    vector<string>  res;

    while (true)
    {
        eof = fgets(buf, 655360, fin);
        if (eof == NULL)
            break;
        res.push_back(eof);
    }

    fclose(fin);
    return res;
}

vector<string>      StringTokenize(string line)
{
    istringstream   strin(line);
    vector<string>  result;
    string          token;

    while (strin >> token)
        result.push_back(token);

    return result;
}

int                 String2Int(string str)
{
    int result = 0;
    int p = 1;
    for (unsigned int i = 0; i < str.length(); i ++)
    {
        if (i == 0 && str[i] == '-')
        {
            p = -1;
            continue;
        }
        if (str[i] >= '0' && str[i] <= '9')
            result = result * 10 + (str[i] - '0');
        else
        {
			if (i == str.length() - 1)
				return result;
            //printf("Error when formating string to integer!\n");
            return -1;
        }
    }
    return result * p;
}

EigenTriangle::EigenTriangle()
{
}

EigenTriangle::~EigenTriangle()
{
}

SparseMatrixType        EigenTriangle::init(const char* fileDir) 
{
    vector<string> inputs = ReadFromFile(fileDir);
    vector<string> tokens;
    int n = (int) inputs.size();
    SparseMatrix<double> adjMatrix(n, n);
    for (unsigned int i = 0; i < inputs.size(); i ++)
    {
        string line = inputs[i];
        tokens = StringTokenize(line);
        int a = i;
        for (unsigned int j = 0; j < tokens.size(); j ++)
        {
            int b = String2Int(tokens[j]);
            adjMatrix.coeffRef(a, b) = 1;
            adjMatrix.coeffRef(b, a) = 1;
        }
    }
    return adjMatrix;
}

// partial reorthogonalization
double                  EigenTriangle::count(SparseMatrixType adjMatrix, int maxIter, double tol)
{
    srand(time(0));
    int n = adjMatrix.rows();
    if (maxIter > n)
        maxIter = n;
    double na = adjMatrix.norm();
    double phi = tol * na;
    double delta = tol * sqrt(na);
    MatrixXd omega = MatrixXd::Zero(n + 2, n + 2);
    for (int i = 1; i < n + 2; i ++)
        omega(i, i - 1) = phi;
    omega += MatrixXd::Identity(n + 2, n + 2);
    //std::cout << omega << std::endl; 
    vector<VectorXd> v;
    v.push_back(VectorXd::Random(n));
    v[0] /= v[0].norm();
    VectorXd alpha(n);
    VectorXd beta(n + 1);
    beta[0] = 0;
    VectorXd w;
    bool flag = false;
    int num = 0;    // reorthogonalization times
    for (int i = 0; i < maxIter; i ++)
    {
        //printf("== Iter %d ===\n", i);
        w = adjMatrix * v[i];
        alpha[i] = w.dot(v[i]);
        if (i == maxIter - 1)
            break;
        w -= alpha[i] * v[i];
        if (i > 0)
            w -= beta[i] * v[i - 1];
        beta[i + 1] = w.norm();
        v.push_back(w / beta[i + 1]);
        if (flag)
        {
            flag = false;
            for (int j = 0; j <= i; j ++)
                v[i + 1] -= v[j].dot(v[i + 1]) * v[j];
            for (int j = 0; j <= i; j ++)
                omega(i + 1, j) = phi;
            /*
            for (int j = 0; j <= i; j ++)
                printf("%.5lf\n", v[i + 1].dot(v[j]));
            */
        }
        else
        {
            omega(i + 1, 0) = 0.0;
            if (i > 0)
                omega(i + 1, 0) = 1.0 / beta(i) * ((alpha(0) - alpha(i)) * omega(i, 0) - beta(i - 1) * omega(i - 1, 0)) + delta;
            for (int j = 1; j <= i; j ++)
            {
                omega(i + 1, j) = 1.0 / beta(i) * (beta(j) * omega(i, j + 1) + (alpha(j) - alpha(i)) * omega(i, j) - beta(j - 1) * omega(i, j - 1) - beta(i - 1) * omega(i - 1, j)) + delta;
            }
        }
        double mx = 0.0;
        for (int j = 0; j <= i; j ++)
            if (mx < fabs(omega(i + 1, j)))
                mx = fabs(omega(i + 1, j));
        if (mx > sqrt(tol))
        {
            for (int j = 0; j <= i; j ++)
                omega(i + 1, j) = phi;
            num ++;
            for (int j = 0; j <= i; j ++)
                v[i + 1] -= v[i + 1].dot(v[j]) * v[j];
            flag = true;
        }
    }
    int k = maxIter;
    MatrixXd T = MatrixXd::Zero(k, k);
    for (int i = 0; i < k; i ++)
    {
        T(i, i) = alpha[i];
        if (i < k - 1)
            T(i, i + 1) = beta[i + 1];
        if (i > 0)
            T(i, i - 1) = beta[i];
    }
    //std::cout << T << std::endl;

    Eigen::EigenSolver<MatrixXd> eigenSolver;
    eigenSolver.compute(T, /* computeEigenvectors = */ false);
    Eigen::VectorXcd eigens = eigenSolver.eigenvalues();
    double res = 0;
    for (int i = 0; i < eigens.size(); i ++)
    {
        std::complex<double> tmp = eigens[i];
        res += pow(tmp.real(), 3);
    }
    res /= 6;
    return res;
}

// full reorthogonalization
double                  EigenTriangle::count(SparseMatrixType adjMatrix, int maxIter)
{
    srand(time(0));
    // check if rows == cols()
    int n = adjMatrix.rows();
    if (maxIter > n)
        maxIter = n;
    vector<VectorXd> v;
    v.push_back(VectorXd::Random(n));
    v[0] = v[0] / v[0].norm();
    VectorXd alpha(n);
    VectorXd beta(n + 1);
    beta[0] = 0;
    VectorXd w;
    for (int i = 0; i < maxIter; i ++)
    {
        w = adjMatrix * v[i];
        alpha[i] = w.dot(v[i]);
        if (i == maxIter - 1)
            break;
        w -= alpha[i] * v[i];
        if (i > 0)
            w -= beta[i] * v[i - 1];
        beta[i + 1] = w.norm();
        v.push_back(w / beta[i + 1]);
        for (int j = 0; j <= i; j ++)
        {
            v[i + 1] = v[i + 1] - v[i + 1].dot(v[j]) * v[j];
        }
    }
    int k = maxIter;
    MatrixXd T = MatrixXd::Zero(k, k);
    for (int i = 0; i < k; i ++)
    {
        T(i, i) = alpha[i];
        if (i < k - 1)
            T(i, i + 1) = beta[i + 1];
        if (i > 0)
            T(i, i - 1) = beta[i];
    }
    //std::cout << T << std::endl;

    Eigen::EigenSolver<MatrixXd> eigenSolver;
    eigenSolver.compute(T, /* computeEigenvectors = */ false);
    Eigen::VectorXcd eigens = eigenSolver.eigenvalues();
    double res = 0;
    for (int i = 0; i < eigens.size(); i ++)
    {
        std::complex<double> tmp = eigens[i];
        res += pow(tmp.real(), 3);
    }
    res /= 6;
    return res;
}

/*
int                 main()
{
    double k = 1.0;
    EigenTriangle* eigenTriangle = new EigenTriangle();
    SparseMatrixType adjMatrix = eigenTriangle -> init("graph.txt");
    double res = eigenTriangle -> count(adjMatrix, adjMatrix.rows() * k, 0.1);
    printf("#triangles: %.5lf\n", res);
    return 0;
}
*/
