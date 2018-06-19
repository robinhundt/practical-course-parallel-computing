#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define N 1024


void init(double *a, double *aTrans, double *result){
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            a[i*N+j]=aTrans[i+j*N]=cos(12*(i+j)/N)*sin(24*(i-j)/N);
            result[i*N+j]=0;
        }
    }
}

void mult(double *a, double *aTrans, double *result){

    #pragma acc data copyin(a[0:N*N]) create(result[0:N*N]) 
    {   
        #pragma acc kernels
        #pragma acc loop gang independent
        for (int i=0; i<N; i++){
            int start = i*N;
            int end =(i+1)*N;
            #pragma acc present_or_copyin(aTrans[start:end])
            {   
                //  #pragma asycn(i)
                #pragma acc loop vector independent
                for(int j=0;j<N;j++){
                    double sum =0;
                    for(int k=0;k<N;k++){
                        sum+= a[i*N+k]*aTrans[i*N+k];
                    }
                    // #pragma  acc wait(i)
                    result[i*N + j] =sum;
                    //#pragma acc atomic update host
                    result[i*N + j] =sum;
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
    double *a,*result, *aTrans;
    a = (double *)malloc(N*N*sizeof(double));
    aTrans = (double *)malloc(N*N*sizeof(double));
    result = (double *)malloc(N*N*sizeof(double));
    init(a,aTrans, result);
    mult(a,aTrans,result);
    printResult(result);
    free(a);
    free(result);
    return 0;
}


    // for(int jj=0;jj<N;jj+= s){
    //     for(int kk=0;kk<N;kk+= s){
    //         //fill chunks
    //         #pragma acc kernel
    //         #pragma acc asycn copyin()
    //         for(int i=0;i<N;i++){
    //             for(int j = jj; j<((jj+s)>N?N:(jj+s)); j++){
    //                 double temp = 0;
    //                 for(int k = kk; k<((kk+s)>N?N:(kk+s)); k++){
    //                         temp += a[i*N +k]*a[k*N+j];
    //                 }
    //                 #pragma acc atomic update
    //                 result[i*N+j] += temp;
    //             }
    //         }
    // }
    // }