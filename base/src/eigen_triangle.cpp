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
#include <cmath>
#include <algorithm>
#include "areig.h"
#include "lnmatrxc.h"
#include "lsmatrxa.h"
#include <set>

using std::pair;
using std::set;
using std::vector;
using std::string;
using std::istringstream;
using std::sort;
using Eigen::VectorXd;
using Eigen::MatrixXd;
using Eigen::Triplet;
using Eigen::SparseVector;
using namespace sae::io;

typedef Triplet<double> T;

EigenTriangle::EigenTriangle(MappedGraph *graph):Solver(graph)
{
    // build adjacent matrix
    int n = graph -> VertexCount();
    vector<Triplet<double> > tripletList;
    //vector<Triplet<double> > edgeList;
    edgeList.clear();
    for (auto iter = graph -> Edges(); iter -> Alive(); iter -> Next())
    {
        vid_t x = iter -> Source() -> GlobalId();
        vid_t y = iter -> Target() -> GlobalId();
        tripletList.push_back(Triplet<double>(x, y, 1));
        tripletList.push_back(Triplet<double>(y, x, 1));
        //edgeList.push_back(make_pair(x, y));
        if (x < y)
        {
            vid_t z = y;
            y = x;
            x = z;
        }
        edgeList.push_back(make_pair(x, y));
        //adjMatrix.coeffRef(x, y) = 1;
        //adjMatrix.coeffRef(y, x) = 1;
    }
    sort(edgeList.begin(), edgeList.end());
    adjMatrix.resize(n, n);
    adjMatrix.setFromTriplets(tripletList.begin(), tripletList.end());
    // try compress mode
    adjMatrix.makeCompressed();
    tripletList.clear();
    //printf("[EigenTriangle] Matrix is ready to go!\n");
}

EigenTriangle::~EigenTriangle()
{
    adjMatrix.resize(0, 0);
}

double                  EigenTriangle::solve(double tol)
{
    int n = (int) graph -> VertexCount();
    int k = tol * n;
    if (k < 10)
        k = 10;
    int nnz = edgeList.size();
    vals = new double[nnz];
    rows = new int[nnz];
    cols = new int[n + 1];
    int row_cnt = 0;
    int col_cnt = 0;
    for (unsigned int i = 0; i < edgeList.size(); i ++)
    {
        int x = edgeList[i].first;
        int y = edgeList[i].second;
        //printf("%d %d\n", x, y);
        if (i > 0 && edgeList[i - 1].first == edgeList[i].first && edgeList[i - 1].second == edgeList[i].second)
            continue;
        rows[row_cnt] = y;
        vals[row_cnt] = 1.0;
        if (i == 0 || (i > 0 && edgeList[i - 1].first != x))
        {
            int last = -1;
            if (i > 0)
                last = edgeList[i - 1].first;
            for (int j = last + 1; j < x; j ++)
                cols[col_cnt ++] = row_cnt;
            cols[col_cnt ++] = row_cnt;
        }
        row_cnt ++;
    }
    nnz = row_cnt;
    for (; col_cnt <= n; col_cnt ++)
    {
        cols[col_cnt] = nnz;
    }
    /*
    for (int i = 0; i < nnz; i ++)
        printf("%d ", rows[i]);
    printf("\n");
    for (int i = 0; i <= n; i ++)
        printf("%d ", cols[i]);
    printf("\n");
    */
    //printf("%d %d %d %d\n", n, nnz, row_cnt, col_cnt);
    //ARluSymMatrix<double> matrix(n, nnz, vals, rows, cols, 'U');
    //ARluSymStdEig<double> dprob(k, matrix, "LM");
    //printf("%d\n", dprob.ConvergedEigenvalues());
    //dprob.FindEigenvalues();

    double *eigVal = new double[k];
    if (k == n)
        k --;
    AREig(eigVal, n, nnz, vals, rows, cols, 'U', k);
    double res = 0;
    //printf("Caculated Eigenvalues: %d\n", dprob.ConvergedEigenvalues());
    //for (int i = 0; i < dprob.ConvergedEigenvalues(); i ++)
    for (int i = 0; i < k; i ++)
    {
        //double eig = dprob.Eigenvalue(i);
        //res += pow(eig, 3) / 6;
        res += pow(eigVal[i], 3) / 6;
        //printf("%.5lf\n", eigVal[i]);
    }
    delete []vals;
    return res;
}

// full reorthogonalization
double                  EigenTriangle::solve(int maxIter)
{
    int n = graph -> VertexCount();
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
    int last = 0;
    printf("%d\n", maxIter);
    for (int i = 0; i < maxIter; i ++)
    {
        if (i * 100 / maxIter > last + 10 || i == maxIter - 1)
        {
            last = (i + 1) * 100 / maxIter;
            std::cout << "\r" << "[EigenTriangle] Processing " << last << "% ..." << std::flush;
        }
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
    printf("\n");
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
        printf("%.5lf\n", tmp.real());
    }
    res /= 6;
    return res;
}

// partial reorthogonalization
double                  EigenTriangle::solve(int maxIter, double tol)
{
    int n = graph -> VertexCount();
    srand(time(0));
    if (maxIter > n)
        maxIter = n;
    double na = adjMatrix.norm();
    double phi = tol * na;
    double delta = tol * sqrt(na);
    vector<Triplet<double> > omega_vector;
    //MatrixXd omega = MatrixXd::Zero(n + 2, n + 2);
    for (int i = 0; i < n + 2; i ++)
    {
        //omega(i, i - 1) = phi;
        if (i > 0)
            omega_vector.push_back(Triplet<double>(i, i - 1, phi)); 
        omega_vector.push_back(Triplet<double>(i, i, 1));
    }
    omega.resize(n + 2, n + 2);
    omega.setFromTriplets(omega_vector.begin(), omega_vector.end());
    //std::cout << omega << std::endl; 
    vector<SparseVector<double> > v;
    //v.push_back(SparseVector::Random(n));
    SparseVector<double> firstV(n);
    for (int i = 0; i < 100; i ++)
        firstV.coeffRef(i % n) = i;
    v.push_back(firstV);
    v[0] /= v[0].norm();
    VectorXd alpha(n);
    VectorXd beta(n + 1);
    beta[0] = 0;
    SparseVector<double> w;
    bool flag = false;
    int num = 0;    // reorthogonalization times
    int last = -1;
    for (int i = 0; i < maxIter; i ++)
    {
        if ((i + 1) * 100 / maxIter > last)
        {
            last = (i + 1) * 100 / maxIter;
            std::cout << "\r" << "[EigenTriangle] Processing " << last << "% ..." << std::flush;
        }
        //printf("== Iter %d ===\n", i);
        w = adjMatrix * v[i];
        alpha.coeffRef(i) = w.dot(v[i]);
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
                omega.coeffRef(i + 1, j) = phi;
            //for (int j = 0; j <= i; j ++)
            //    printf("%.5lf\n", v[i + 1].dot(v[j]));
        }
        else
        {
            omega.coeffRef(i + 1, 0) = 0.0;
            if (i > 0)
            {
                omega.coeffRef(i + 1, 0) = 1.0 / beta(i) * ((alpha(0) - alpha(i)) * omega.coeffRef(i, 0) - beta(i - 1) * omega.coeffRef(i - 1, 0)) + delta;
            }
            for (int j = 1; j <= i; j ++)
            {
                omega.coeffRef(i + 1, j) = 1.0 / beta(i) * (beta(j) * omega.coeffRef(i, j + 1) + (alpha(j) - alpha(i)) * omega.coeffRef(i, j) - beta(j - 1) * omega.coeffRef(i, j - 1) - beta(i - 1) * omega.coeffRef(i - 1, j)) + delta;
            }
        }
        double mx = 0.0;
        for (int j = 0; j <= i; j ++)
            if (mx < fabs(omega.coeffRef(i + 1, j)))
                mx = fabs(omega.coeffRef(i + 1, j));
        if (mx > sqrt(tol))
        {
            for (int j = 0; j <= i; j ++)
                omega.coeffRef(i + 1, j) = phi;
            num ++;
            for (int j = 0; j <= i; j ++)
                v[i + 1] -= v[i + 1].dot(v[j]) * v[j];
            flag = true;
        }
    }
    printf("\n");
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
    eigenSolver.compute(T, false);
    Eigen::VectorXcd eigens = eigenSolver.eigenvalues();
    double res = 0;
    for (int i = 0; i < eigens.size(); i ++)
    {
        std::complex<double> tmp = eigens[i];
        res += pow(tmp.real(), 3) / 6;
        std::cout << i << ": " << tmp << std::endl;
    }
    //res /= 6;
    return res;
}

