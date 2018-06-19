#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define N 1024


void init(double *a, double *result){
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            a[i*N+j]=cos(12*(i+j)/N)*sin(24*(i-j)/N);
            result[i*N+j]=0;
        }
    }
}

void mult(double *a, double *result){
    int s =N/2;
    double *chunk1= (double*)malloc(s*s*sizeof(double));
    double *chunk2= (double*)malloc(s*s*sizeof(double));
    
    for(int jj=0;jj<N;jj+= s){
        for(int kk=0;kk<N;kk+= s){
            //fill chunks
            #pragma acc kernel
            #pragma acc asycn copyin()
            for(int i=0;i<N;i++){
                for(int j = jj; j<((jj+s)>N?N:(jj+s)); j++){
                    double temp = 0;
                    for(int k = kk; k<((kk+s)>N?N:(kk+s)); k++){
                            temp += a[i*N +k]*a[k*N+j];
                    }
                    #pragma acc atomic update
                    result[i*N+j] += temp;
                }
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
    double *a,*result;
    a = (double *)malloc(N*N*sizeof(double));
    result = (double *)malloc(N*N*sizeof(double));
    init(a, result);
    mult(a,result);
    printResult(result);
    free(a);
    free(result);
    return 0;
}
