#include <stdio.h>
#include <stdlib.h>
#include <math.h>

bool is_power_of_two(unsigned int x)
{
    // https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2/600306#600306
    return (x != 0) && ((x & (x - 1)) == 0);
}


template <unsigned int blockSize>
__global__ void reduce(float *g_in, double *g_out, unsigned int n)
{
    // largely inspired by
    // http://developer.download.nvidia.com/compute/cuda/1.1-Beta/x86_website/projects/reduction/doc/reduction.pdf

    extern __shared__ double s_data[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*(blockSize * 2) + tid;
    unsigned int gridSize = blockSize * 2 * gridDim.x;
    s_data[tid] = 0;
    
    while(i < n) {
        s_data[tid] += g_in[i] + g_in[i+blockSize];
        i += gridSize;
    }
    __syncthreads();

    // following is the completely unrolled 
    // loop for the reduction of the shared array
    if(blockSize >= 512) {
        if(tid < 256)
            s_data[tid] += s_data[tid + 256];
        __syncthreads();
    }
    if(blockSize >= 256) {
        if(tid < 128)
            s_data[tid] += s_data[tid + 128];
        __syncthreads();
    }
    if(blockSize >= 128) {
        if(tid < 64)
            s_data[tid] += s_data[tid + 64];
        __syncthreads();
    }

    if(tid < 32) {
        // the code on the slides misses the sync after the partial
        // recution of the shared array
        if(blockSize >= 64) {s_data[tid] += s_data[tid + 32]; __syncthreads();}
        if(blockSize >= 32) {s_data[tid] += s_data[tid + 16]; __syncthreads();}
        if(blockSize >= 16) {s_data[tid] += s_data[tid + 8]; __syncthreads();}
        if(blockSize >= 8) {s_data[tid] += s_data[tid + 4]; __syncthreads();}
        if(blockSize >= 4) {s_data[tid] += s_data[tid + 2]; __syncthreads();}
        if(blockSize >= 2) {s_data[tid] += s_data[tid + 1]; __syncthreads();}
    }
    if(tid == 0) {
        g_out[blockIdx.x] = s_data[0];
    }
}


int main(int argc, char const *argv[])
{
    if(argc <= 1) {
        printf("Please call the program with an integer that is a"
                " power of two.");
        return 1;
    }
    unsigned int N = atoi(argv[1]);
    if(!is_power_of_two(N)) {
        printf("Please call the program with an integer that is a"
                " power of two.");
        return 1;
    }

    float *in;
    double *out; 
    cudaMallocManaged(&in, sizeof *in * N);
    
    // initialize array on host with ones
    for(int i=0; i<N; i++) {
        in[i] = 1;
    }


    int threadCountGrid = ceil((double)N / log2(N));
    int dimBlock = 512;
    printf("Using %d threads in total\n", max(threadCountGrid, dimBlock));
    printf("Divided on blocks of size %d\n", dimBlock);    
    int dimGrid = min(64, threadCountGrid / 512);
    printf("With %d blocks\n", dimGrid);    
    size_t smemSize = sizeof *out * dimBlock;
    cudaMallocManaged(&out, sizeof *out * dimGrid);

    switch(dimBlock) {
        case 512:
            reduce<512><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 256:
            reduce<256><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 128:
            reduce<128><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 64:
            reduce<64><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 32:
            reduce<32><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 16:
            reduce<16><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 8:
            reduce<8><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 4:
            reduce<4><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 2:
            reduce<2><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
        case 1:
            reduce<1><<<dimGrid, dimBlock, smemSize>>>(in, out, N);
            break;
    }

    cudaDeviceSynchronize();    

    // printf("Partial sums from reduction on GPU:\n");
    // for(int i=0; i<dimGrid; i++) {
    //     printf("%f,", out[i]);
    // }
    // printf("\n");

    double sum = 0;
    printf("Complete sum-reduction: ");
    for(int i=0; i<dimGrid; i++)
        sum += out[i];
    printf("%f\n", sum);

    cudaFree(in);
    cudaFree(out);

    return 0;


}
