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
using namespace sae::io;

EigenTriangle::EigenTriangle(MappedGraph *graph):Solver(graph)
{
}

EigenTriangle::~EigenTriangle()
{

}

// full reorthogonalization
double                  EigenTriangle::solve(int maxIter)
{
    // build adjacent matrix
    int n = graph -> VertexCount();
    SparseMatrix<double> adjMatrix(n, n);
    for (auto iter = graph -> Edges(); iter -> Alive(); iter -> Next())
    {
        vid_t x = iter -> Source() -> GlobalId();
        vid_t y = iter -> Target() -> GlobalId();
        adjMatrix.coeffRef(x, y) = 1;
        adjMatrix.coeffRef(y, x) = 1;
    }
    srand(time(0));
    // check if rows == cols()
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

// partial reorthogonalization
double                  EigenTriangle::solve(int maxIter, double tol)
{
    // build adjacent matrix
    int n = graph -> VertexCount();
    SparseMatrix<double> adjMatrix(n, n);
    for (auto iter = graph -> Edges(); iter -> Alive(); iter -> Next())
    {
        vid_t x = iter -> Source() -> GlobalId();
        vid_t y = iter -> Target() -> GlobalId();
        adjMatrix.coeffRef(x, y) = 1;
        adjMatrix.coeffRef(y, x) = 1;
    }
    srand(time(0));
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

