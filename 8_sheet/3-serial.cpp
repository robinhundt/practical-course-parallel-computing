#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <cstring>
#include <algorithm>


using namespace std;

void initMatrix(float *A, int n = 512) {
    for(int i=0; i<n; i++) {
        for(int  j=0; j<n; j++) {
            if(i == 0 || i == n-1 || j == 0 || j == n-1)
                A[i*n+j] = cos((4.0 * M_PI * i) / (n - 1.0)) \
                         * cos((4.0 * M_PI * j) / (n - 1.0));
            else
                A[i*n+j] = 0;
        }
    }
}

int jacobiIteration(float *A, int n = 512, float eps=0.01) {
    size_t size = sizeof *A *n*n;
    float *A_tmp = (float *) malloc(size);
    float maxeps = eps;
    int iterations = 0;
    while(maxeps >= eps) {
        iterations++;
        memcpy(A_tmp, A, size);    
        maxeps = 0;
        for(int i=1; i<n-1; i++) {
            for(int j=1; j<n-1; j++) {
                A[i*n+j] = (A_tmp[(i-1)*n+j] + A_tmp[(i+1)*n+j] \
                         + A_tmp[i*n+j-1] + A_tmp[i*n+j+1]) / 4.0;
                maxeps = max(maxeps, abs(A[i*n+j] - A_tmp[i*n+j]));
            }
        }
    }
    free(A_tmp);
    return iterations;
}

void printMatrix(float *A, int n = 512) {
    for(int i=0; i<n; i++) {
        for(int  j=0; j<n-1; j++) {
            printf("%f;", A[i*n+j]);
        }
        printf("%f\n", A[i*n+n-1]);        
    }
}

int main(int argc, char *argv[]) {
    int N;
    if (argc > 1) N = atoi(argv[1]); else N = 512; 
    fprintf(stderr, "Using grid size %dx%d\n", N, N);    
    float *A = (float *) malloc(sizeof *A *N*N);
    initMatrix(A, N);
    printMatrix(A, N);
    int iterations = jacobiIteration(A, N);
    fprintf(stderr, "iterations: %d\n", iterations);
    printMatrix(A, N);
    free(A);
    return 0;
}