#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double multiply(double a[], double b[], int n);
//Execute as: ./e1 6   (here 6 is command line param for the size of the vectors to be multiplied)
int main (int argc, const char * argv[])
{
    double sum;
    if (argc!=2) {
      printf("Please hand te vector size as a command line argument");
      return 1;
    }
    int n=atoi(argv[1]);//Input from command line argument. Restricting vectors to same length.
    double a [n], b [n];
    int i;
    
    //Snippet#1:
    #pragma omp parallel
    printf("Hello, World! from Thread %d\n",omp_get_thread_num());

    //populating the vectors
    for (i = 0; i < n; i++) {
        a [i] = i * 0.25;
        b [i] = i * 1.5;
    }
        
    sum = 0.0;
    //Snippet#2:
    #pragma omp parallel
    {
        double local_sum = 0.0;
        local_sum = multiply(a,b,n);
        #pragma omp critical
        sum += local_sum;
    }
    printf ("Snippet#2: sum =  %f \n", sum);
    
    
    sum = 0.0;    
    //Snippet#3:
    #pragma omp parallel reduction(+:sum)
    {
        sum += multiply(a,b,n);
    }
    printf ("Snippet#3: sum =  %f \n", sum);

    return 0;
}

double multiply(double a[], double b[], int n)
{
    double local_sum = 0.0;
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();    
    int patchsize = n/thread_count;
    int gap = n - patchsize * thread_count;
    
    int start = my_rank * patchsize;
    
    if (gap != 0) {
        if (my_rank < gap) {
            start += my_rank;
        } else {
            start += gap;
        }
    }
    
    int end = start+patchsize;
    if (gap != 0) {
        if (my_rank < gap) {
            end++;
        }
    }
    
    for ( ; start < end; start++ ) {
        local_sum += a[start]*b[start];
    }
    return local_sum;
}


