
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/**
 *  a) #pragma omp parallel:  designates a block that is executed by multiple
 *      threads. When a master thread reaches the block, a team of threads is
 *      created and each thread executes a duplicate of  the code.
 *     #pragma omp for: can be used inside a parallel region. Designates that 
 *      the iterations of the following loop should be executed in parallel
 *      by the team of threads
 *     #pragma omp parallel for: convenience method that behaves identically
 *      to a parallel directive followed immediately by a for directive
 * 
 *  b) The problem of data/loop dependency exists when loops that access shared data
 *      are executed in parallel.
 *      In the program there exists a `read-after-write hazard` since the array must 
 *      be first written to at the position x[i-1] before it can be read and the
 *      next factorial can be computed
 **/ 


void factorial(int x[], int n);

int main ()
{
    int arr[8];
    factorial(arr, 8);
    
    return 0;
}

void factorial(int x[], int n)
{
    int i,j,thread_id=42;
    /**
     * Add the ordered clause since there is a data dependency 
     * inside the else clause of the for loop
     **/
    #pragma omp parallel for ordered num_threads(4) \
        default(none) private(i,j, thread_id) shared (n,x) schedule(static, 1)
    for(i=0; i<n; i++)
    {
        if(i<2)
            x[i] = 1;
        else
            #pragma omp ordered     // the following code will be executed in the order of the loop 
            x[i] = x[i-1]*i;
                
        thread_id = omp_get_thread_num();
        printf("Thread id # %d computed factorial(%d) = %d \n",
            thread_id, i, x[i]);
    }
    
    for(j=0; j<n; j++)
        printf("%d\t",x[j]);
    printf("\n");
}


