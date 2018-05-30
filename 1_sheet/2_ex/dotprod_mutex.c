#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define VECLEN 100000

/**
 * a and b are pointer to  double vectors of size VECLEN * NUM_THREADS
 * sum is the sum of entries of the vector that results from the 
 * pointwise multiplication of a and b
 **/
typedef struct {
    double *a;
    double *b;
    double sum;
    int veclen;
} DotData;

// declare one global struct
DotData dotstr;
// declare global mutex used to coordinate access to dotstr
pthread_mutex_t mutexsum;

// dotprod expects a single long value as arg. that represents 
// the offset of the vectors to be multiplied and summed
// from the start of a and b respectively
// the function will multiply and sum the elements of 
// x[offset * veclen] to x[offset * len + len] for a and b
void *dotprod(void *arg) {
    long offset = (long) arg;
    int len = dotstr.veclen;

    int start = offset * len;
    int end = start + len;
    double *x = dotstr.a;
    double *y = dotstr.b;

    double sum = 0;
    for(int i=start; i<end; i++)
        sum += x[i] * y[i];
    // lcok mutex in order to prevent undefined behaviour when
    // accessing shared global dotstr.sum in parallel
    pthread_mutex_lock(&mutexsum);
    dotstr.sum += sum;
    printf("Thread %ld did %d to %d: sum=%f global sum=%f\n",
            offset, start, end, sum, dotstr.sum);
    pthread_mutex_unlock(&mutexsum);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    void *status;
    // allocate memory for vectors a and b with NUM_THREADS * VECLEN entries resp.
    double *a = malloc(sizeof *a * NUM_THREADS * VECLEN);
    double *b = malloc(sizeof *b * NUM_THREADS * VECLEN);
    for(int i=0; i<VECLEN*NUM_THREADS; i++)
        a[i] = b[i] = 1;
    
    // initialize global struct
    dotstr.veclen = VECLEN;
    dotstr.a = a;
    dotstr.b = b;
    dotstr.sum = 0;

    // initialize mutex, passing NULL as arg will initialize the mutex with default params 
    pthread_mutex_init(&mutexsum, NULL);

    for(long i=0; i<NUM_THREADS; i++) 
        pthread_create(&threads[i], NULL, dotprod, (void *) i);
    
    // block execution of main thread until each thread has terminated
    for(int i=0; i<NUM_THREADS; i++)
        pthread_join(threads[i], &status);
    
    printf("Sum = %f\n", dotstr.sum);
    
    // cleanup
    free(a);
    free(b);
    pthread_mutex_destroy(&mutexsum);
    return 0;
}