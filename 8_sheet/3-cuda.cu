#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <cstring>
#include <algorithm>


using namespace std;

__global__ void initMatrix(float *A, int n = 512) {
    uint i = (blockIdx.y * blockDim.y) + threadIdx.y;
    uint j = (blockIdx.x * blockDim.x) + threadIdx.x;
    // printf("j: %d, i: %d\n", i, j);
    if(i == 0 || i == n-1 || j == 0 || j == n-1)
        A[i*n+j] = cosf((4.0 * M_PI * i) / (n - 1.0)) \
                    * cosf((4.0 * M_PI * j) / (n - 1.0));
    else
        A[i*n+j] = 0;
}

 __global__ void jacobiIteration(float *A, int n = 512, int iterations = 100) {
    float A_nghbrs[4];
    uint i = (blockIdx.y * blockDim.y) + threadIdx.y;
    uint j = (blockIdx.x * blockDim.x) + threadIdx.x;
    for(int k=0; k<iterations; k++) {
        __syncthreads();
        A_nghbrs[0] = A[(i-1)*n+j];
        A_nghbrs[1] = A[(i+1)*n+j];
        A_nghbrs[2] = A[i*n+j-1];
        A_nghbrs[3] = A[i*n+j+1];
        __syncthreads();
        A[i*n+j] = (A_nghbrs[0] + A_nghbrs[1] + A_nghbrs[2] + A_nghbrs[3]) / 4.0;
    }
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
    unsigned int N;
    int iterations = 36;
    if (argc > 1)
        N = atoi(argv[1]);
    else
        N = 512;
    fprintf(stderr, "Using grid size %dx%d\n", N, N);                
    float *A;
    cudaMallocManaged(&A, sizeof *A *N*N);
    dim3 nt(8,8);
    dim3 nb(N/nt.x, N/nt.y);
    initMatrix<<<nb, nt>>>(A, N);
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) 
        printf("Error: %s\n", cudaGetErrorString(err));
    cudaDeviceSynchronize();        
    printMatrix(A, N);
    jacobiIteration<<<nt, nb>>>(A, N, iterations);
    printMatrix(A, N);
    fprintf(stderr, "iterations: %d\n", iterations);    
    cudaFree(A);
    return 0;
}