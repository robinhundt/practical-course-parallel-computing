#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define N 1024

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
    double *a,*result;
    a = (double *)malloc(N*N*sizeof(double));
    result = (double *)malloc(N*N*sizeof(double));
    #pragma acc data create(a[0:N*N]) copyout (result[0:N*N])
    {   
        //init
        #pragma acc kernels
        #pragma acc loop
        for(int i=0;i<N;i++){
            for(int j=0;j<N;j++){
                a[i*N+j]=cos(12*(i+j)/N)*sin(24*(i-j)/N);
            }
        }
        //mult
        #pragma acc kernels
        #pragma acc loop gang independent
            for(int i = 0;i<N;i++){
                #pragma acc loop vector independent
                for(int j=0; j<N;j++){
                    double sum = 0;
                    for(int k=0;k<N;k++){
                        sum+= a[i*N+k]*a[j+k*N];
                    }
                    result[i*N + j] =sum;
                }
            }
    } 

    printResult(result);
    free(a);

    free(result);
    return 0;
}
