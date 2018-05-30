#include "stdio.h"
#include "stdlib.h"
#include "omp.h"

//Execute as: ./q2 6 (command line param assigns size of two square matrices to be multiplied)
int main(int argc, char *argv[])
{
    long ** m1, ** m2, ** prod;
    
    int i,j,k,r1,c1,r2,c2, result=0, thread_id=42, number_of_threads=23;
    r1 = c1 = r2 = c2 = atoi(argv[1]); //Input from command line argument. Restricting to square matrices.
    /*
    printf("Enter number of rows and columns of first matrix\n");
    scanf("%d%d",&r1,&c1);
    printf("Enter number of rows and columns of second matrix\n");
    scanf("%d%d",&r2,&c2);
    */
    
    
    if(r2==c1)
    {
        //Allocating memory
        m1 = (long **) malloc(r1 * sizeof(long *));
        for(i = 0; i < r1; i++)
        {
            m1[i] = (long *) malloc(c1 * sizeof(long));
        }
        
        m2 = (long **) malloc(r2 * sizeof(long *));
        for(i = 0; i < r2; i++)
        {
            m2[i] = (long *) malloc(c2 * sizeof(long));
        }
        
        prod = (long **) malloc(r1 * sizeof(long *));
        for(i = 0; i < r1; i++)
        {
            prod[i] = (long *) malloc(c2 * sizeof(long));
        }
        
        //Populate the matrices & print out to see it is correctly allocated memory.
        // printf("Populating the matrices ...\n");
        // printf("Matrix 1:\n");
        for(i=0; i<r1; i++)
        {
            for(j=0; j<c1; j++){
                m1[i][j] = j+1;
                // printf("%lu\t",m1[i][j]);
            }
            // printf("\n");
        }
        // printf("Matrix 2:\n");
        for(i=0; i<r2; i++)
        {
            for(j=0; j<c2; j++){
                m2[i][j] = j+2;
                // printf("%lu\t",m2[i][j]);
            }
            // printf("\n");
        }
        // printf("Matrices populated, proceeding to multiply ...\n");

        double start = omp_get_wtime();

        #pragma omp parallel shared(i, m1, m2, c1, c2, prod) \
            private(j, k, thread_id)
        {
            double start = omp_get_wtime();
            #pragma omp for
            for(i=0;i<r1;i++)
            {
                thread_id = omp_get_thread_num();
                number_of_threads = omp_get_num_threads();
                {
                    for(j=0;j<c2;j++)
                    {
                        prod[i][j] = 0;
                        for(k=0;k<c1;k++)
                            prod[i][j] += m1[i][k]*m2[k][j];
                        // printf("Thread#%d of %d computed prod[%d][%d]=%lu\t\n",thread_id,number_of_threads,i,j,prod[i][j]);
                    }
                }
                // printf("\n");
            }
            double end = omp_get_wtime();
            // uncomment this to get individual thread execetion times
            // printf("Thread# %d: Time Taken =%f sec\n", thread_id, end-start);            
        }

        double end = omp_get_wtime(); 
        double elapsed = end - start;
        printf("%f",elapsed);
        
        //Print the product
        // printf("\nMultiplied Matrix:\n");
        // for(i=0; i<r1; i++)
        // {
        //     for(j=0; j<c2; j++){
        //         printf("%lu\t",prod[i][j]);
        //     }
        //     printf("\n");
        // }
        
        //Free memory
        for(i=0;i<r1;i++) {
            free(m1[i]);
        }
        free(m1);
        for(i=0;i<r2; i++) {
            free(m2[i]);
        }
        free(m2);
        for(i=0;i<r1; i++) {
            free(prod[i]);
        }
        free(prod);
        
    }
    else
    {
        printf("Matrix multiplication cannot be done");
    }
    
    return 0;
}

