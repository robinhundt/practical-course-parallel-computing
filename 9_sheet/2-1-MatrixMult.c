#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define N 1024


void init(double *a, double *b){
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            a[i*N+j]=b[i*N+j]=cos(12*(i+j)/N)*sin(24*(i-j)/N);
    }

}

void mult(double *a, double *b, double *result){
    #pragma acc data copyin (a[0:N * N], b[0:N * N]) copyout (result [0: N * N])
    {   
        #pragma acc kernels
        #pragma acc loop gang independent
        for(int i = 0;i<N;i++){
            #pragma acc loop vector independent
            for(int j=0; j<N;j++){
                double sum = 0;
                for(int k=0;k<N;k++){
                    sum+= a[i*N+k]*b[j+k*N];
                }
                result[i*N + j] =sum;
            }
        }
    }
    
}

void printResult(double *result){
    for(int i = 0;i<N;i++){
        for(int j=0; j<N;j++){
            if(j>0)
                printf(" %f",result[i*N+j]);
            else
                printf("%f",result[i*N+j]);
        }
        printf("\n");
    }
}



int main(int argc, char const *argv[])
{   
    double *a,*b,*result;
    a = (double *)malloc(N*N*sizeof(double));
    b = (double *)malloc(N*N*sizeof(double));
    result = (double *)malloc(N*N*sizeof(double));
    init(a,b);
    mult(a,b,result);
    printResult(result);
    free(a);
    free(b);
    free(result);
    return 0;
}
