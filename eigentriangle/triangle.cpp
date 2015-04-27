#include "matrix.h"
#include "tools_eigen.h"
#include <cmath>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <ctime>

using std::vector;

// sample a random number within [0, 1]
double          Random()
{
    return 1.0 * rand() / RAND_MAX;
}

// VTdV
double          VectorProduct(vector<double> A, vector<double> B)
{
    if (A.size() != B.size())
        return -1;
    double res = 0.0;
    for (unsigned int i = 0; i < A.size(); i ++)
        res += A[i] * B[i];
    return res;
}

// VmV
vector<double>  VectorMinus(vector<double> A, vector<double> B, double c)
{
    vector<double> res;
    if (A.size() != B.size())
        return res;
    for (unsigned int i = 0; i < A.size(); i ++)
        res.push_back(A[i] - B[i] * c);
    return res;
}

// VdS
vector<double>  VectorProduct(vector<double> A, double B)
{
    vector<double> res;
    for (unsigned int i = 0; i < A.size(); i ++)
        res.push_back(A[i] * B);
    return res;
}

int     PrintVector(vector<double> A)
{
    for (unsigned int i = 0; i < A.size(); i ++)
        printf("%.5lf ", A[i]);
    printf("\n");
    return 0;
}

int     ForceTriangle(Matrix* A)
{
    time_t startTime;
    startTime = clock();
    int res = 0;
    int n = A -> GetRowNum();
    for (int i = 0; i < n; i ++)
    {
        for (int j = i + 1; j < n; j ++)
        {
            for (int k = j + 1; k < n; k ++)
                if (A -> Get(i, j) == 1 && A -> Get(i, k) == 1 && A -> Get(j, k) == 1)
                    res ++;
        }
    }
    time_t endTime = clock();
    printf("Time cost for BruteForce: %.5lf\n", (endTime - startTime + 0.0) / CLOCKS_PER_SEC);
    return res;
}

// Lanczos method to calculate eigen values
double      EigenTriangle(Matrix* A, int m)
{
    time_t startTime = clock();
    srand(time(0));
    int n = A -> GetRowNum();
    double** v = new double*[m];
    for (int i = 0; i < m; i ++)
        v[i] = new double[n];
    double s = 0;
    // generate the first Lanczos vector randomly
    for (int i = 0; i < n; i ++)
    {
        v[0][i] = Random();
        s += v[0][i] * v[0][i];
    }
    s = sqrt(s);
    for (int i = 0; i < n; i ++)
        v[0][i] /= s;

    // alpha and beta are used to construct the tridiagonal matrix T
    double* w = new double[n];
    double* alpha = new double[n];
    double* beta = new double[n + 1];
    beta[0] = 0;
    int iter = 0;
    for (int i = 0; i < m; i ++)
    {
        /*
        if (i * 100 / m >= iter + 10)
        {
            iter = i * 100 / m;
            printf("Processing %d%%...\n", iter);
        }
        */
        for (int j = 0; j < n; j ++)
        {
            w[j] = 0;
            for (int k = 0; k < n; k ++)
                w[j] += A -> Get(j, k) * v[i][k];
        }
        /*
        printf("Lanczos vector: ");
        for (int j = 0; j < n; j ++)
            printf("%.5lf ", v[i][j]);
        printf("\n");
        */
        alpha[i] = 0;
        for (int j = 0; j < n; j ++)
            alpha[i] += w[j] * v[i][j];
        if (i == m - 1)
            break;
        for (int j = 0; j < n; j ++)
        {
            w[j] = w[j] - alpha[i] * v[i][j];
            if (i > 0)
                w[j] -= beta[i] * v[i - 1][j];
        }
        beta[i + 1] = 0;
        for (int j = 0; j < n; j ++)
            beta[i + 1] += w[j] * w[j];
        beta[i + 1] = sqrt(beta[i + 1]);
        /*
        if (fabs(beta[i + 1]) <= 1e-10)
        {
            m = i + 1;
            break;
        }
        */
        if (beta[i + 1] < 0)
            beta[i + 1] *= -1;
        for (int j = 0; j < n; j ++)
        {
            v[i + 1][j] = w[j] / beta[i + 1];
        }
        
        // recover orthogonality basis
        for (int j = 0; j < i; j ++)
        {
            double dp = 0.0;
            for (int k = 0; k < n; k ++)
                dp += v[i + 1][k] * v[j][k];
            for (int k = 0; k < n; k ++)
                v[i + 1][k] -= dp * v[j][k];
        }
    }
    double* T = new double[m * m];
    FILE* fout = fopen("../../../eigenvalues-master/T.txt", "w");
    for (int i = 0; i < m; i ++)
    {
        for (int j = 0; j < m; j ++)
            T[i * m + j] = 0;
        T[i * m + i] = alpha[i];
        if (i > 0)
            T[i * m + i - 1] = beta[i];
        if (i < m - 1)
            T[i * m + i + 1] = beta[i + 1];
        for (int j = 0; j < m; j ++)
            fprintf(fout, "%.5lf ", T[i * m + j]);
        fprintf(fout, "\n");
    }
    qrAlmostUpperTriangle(T, m);
    traceMatrix(T, m);
    /*
    for (int i = 0; i < m; i ++)
    {
        for (int j = 0; j < m; j ++)
            printf("%.5lf ", T[i * m + j]);
        printf("\n");
    }
    */
    double* eigens = new double[m];
    searchEigenValues(T, eigens, m);
    double sum = 0;
    //printf("Eigenvalues: \n");
    for (int i = 0; i < m; i ++)
    {
        //printf("%.5lf\n", eigens[i]);
        sum += pow(eigens[i], 3);
    }
    time_t endTime = clock();
    printf("Time cost for EigenTriangle: %.5lf\n", (endTime - startTime + 0.0) / CLOCKS_PER_SEC);
    return sum / 6;
}

Matrix*     GenerateNetwork(int n, double p)
{
    srand(time(0));
    Matrix* A = new Matrix();
    A -> Create(n, n);
    int edgeNum = 0;
    for (int i = 0; i < n; i ++)
    {
        for (int j = i + 1; j < n; j ++)
        {
            double v = Random();
            int s = 0;
            if (v < p)
                s = 1;
            edgeNum += s;
            A -> Set(i, j, s);
            A -> Set(j, i, s);
        }
    }
    printf("#Edge: %d\n", edgeNum);
    return A;
}

int         main()
{
    int N = 10000;              // number of nodes
    double p = 0.7;             // probability of generating a edge
    double k = 0.01;            // top k*n eigenvalues we care

    Matrix* A = new Matrix();   // adjcant matrix
    A = GenerateNetwork(N, p);
    A -> Print("matrix.txt");

    // brute force method
    printf("==== BruteForce ====\n");
    int force = ForceTriangle(A);
    printf("#triangles: %d\n", force);
    printf("\n");

    // EigenTriangle method
    printf("==== EigenTriangle ====\n");
    double res = EigenTriangle(A, k * N);
    printf("#triangles: %.5lf\n", res);
    printf("Error ratio: %.5lf\n", fabs(res - force) / force);
    printf("\n");
}
