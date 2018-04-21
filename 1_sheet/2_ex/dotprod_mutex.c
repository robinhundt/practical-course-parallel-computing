#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define VECLEN 100000

typedef struct {
    double *a;
    double *b;
    double sum;
    int veclen;
} DotData;


DotData dotstr;
pthread_t threads[NUM_THREADS];
pthread_mutex_t mutexsum;

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
    
    pthread_mutex_lock(&mutexsum);
    dotstr.sum += sum;
    printf("Thread %ld did %d to %d: sum=%f global sum=%f\n",
            offset, start, end, sum, dotstr.sum);
    pthread_mutex_unlock(&mutexsum);
    pthread_exit(NULL);
}

int main() {
    void *status;
    double *a = malloc(sizeof *a * NUM_THREADS * VECLEN);
    double *b = malloc(sizeof *b * NUM_THREADS * VECLEN);
    for(int i=0; i<VECLEN*NUM_THREADS; i++) {
        a[i] = b[i] = 1;
    }
    dotstr.veclen = VECLEN;
    dotstr.a = a;
    dotstr.b = b;
    dotstr.sum = 0;

    pthread_mutex_init(&mutexsum, NULL);

    for(long i=0; i<NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, dotprod, (void *) i);
    }
    for(int i=0; i<NUM_THREADS; i++) {
        pthread_join(threads[i], &status);
    }
    printf("Sum = %f\n", dotstr.sum);
    free(a);
    free(b);
}