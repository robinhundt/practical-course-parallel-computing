#include <stdio.h>
#include <stdlib.h>

int is_power_of_two(int x)
{
    // https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2/600306#600306
    return (x != 0) && ((x & (x - 1)) == 0);
}

__global__ void  sum_arr(float *arr, int n, double *res) { 
    for(int i=0; i<n; i++)
        *res += arr[i];
}

int main(int argc, char const *argv[])
{
    const int N = argc-1;    
    if(N == 0 || !is_power_of_two(N)) {
        printf("Please call the program with the 2^N array elements "
                "to be summed as arguments.\n");
        return 1;
    }

    float *arr;
    double *sum; 
    cudaMallocManaged(&arr, sizeof *arr * N);
    cudaMallocManaged(&sum, sizeof *sum);
    *sum = 0;
    
    // initialize array on host with program arguments
    for(int i=0; i<N; i++) {
        arr[i] = atof(argv[i+1]);
    }
    sum_arr<<<1, 1>>>(arr, N, sum);
    cudaDeviceSynchronize();

    cudaFree(arr);

    printf("Result is: %f\n", *sum);
    return 0;
}
