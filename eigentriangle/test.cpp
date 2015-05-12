#include "eigen_triangle.h"
#include "matrix.h"
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

double  ForceTriangle(Matrix* A)
{
    time_t startTime;
    startTime = clock();
    double res = 0;
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


Matrix*     GenerateNetwork(int n)
{
    srand(time(0));
    Matrix* A = new Matrix();
    A -> Create(n, n);
    int* edgeCount = new int[n];
    for (int i = 0; i < n; i ++)
        edgeCount[i] = 0;
    int edgeNum = 0;
    for (int i = 0; i < n; i ++)
    {
        for (int j = 0; j < n; j ++)
        {
            double v = Random();
            double p = 1;
            if (edgeNum > 0)
                p = (edgeCount[i] + 1.0) / (edgeNum + 1.0);
            if (v <= p * 2)
            {
                A -> Set(i, j, 1);
                edgeCount[j] ++;
                edgeNum ++;
            }
        }
    }
    delete []edgeCount;
    return A;
}

Matrix*     GenerateNetwork(int n, double p)
{
    //srand(time(0));
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
    int N = 5000;               // number of nodes
    double p = 0.3;             // probability of generating a edge
    double k = 0.01;            // top k*n eigenvalues we care

    Matrix* A = new Matrix();   // adjcant matrix
    A = GenerateNetwork(N, p);
    //A = GenerateNetwork(N);
    A -> Print("graph.txt");

    // brute force method
    printf("==== BruteForce ====\n");
    double force = 1;//ForceTriangle(A);
    printf("#triangles: %.0lf\n", force);
    printf("\n");

    // EigenTriangle method
    printf("==== EigenTriangle ====\n");
    EigenTriangle* eigenTriangle = new EigenTriangle();
    SparseMatrixType adjMatrix = eigenTriangle -> init("graph.txt");
    time_t startTime = clock();
    double res = eigenTriangle -> count(adjMatrix, N * k, 1);
    time_t endTime = clock();
    printf("Time cost for EigenTriangle: %.5lf\n", (endTime - startTime + 0.0) / CLOCKS_PER_SEC);
    printf("#triangles: %.5lf\n", res);
    printf("Error ratio: %.5lf\n", fabs(res - force) / force);
    printf("\n");
}
