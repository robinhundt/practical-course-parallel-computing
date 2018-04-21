#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define NUMBERS 12800000000

// __int128 is a gcc extension
unsigned __int128 sum = 0;
pthread_mutex_t mutex_sum;

typedef struct {
    long start;
    long end;
    int tid;
} interval;

void print_uint128(unsigned __int128 n) {
    if (n == 0) {
      return;
    }

    print_uint128(n/10);
    putchar(n%10+'0');
}

void *add(void *arg) {
    interval *my_interval = (interval *) arg;
    long start = my_interval->start;
    long end = my_interval->end;

    printf("Thread: %d, Interval: %ld - %ld\n", my_interval->tid, start, end);

    pthread_mutex_lock(&mutex_sum);
    // for(long j=start; j<=end; j++)
    //     sum += j;
    // is equivalent to the following:
    sum += ( (unsigned __int128) end * end + end) / 2 - ( (unsigned __int128) start * start - start) / 2;
    printf("Thread: %d, sum at = ", my_interval->tid);    
    print_uint128(sum);
    putchar('\n');
    pthread_mutex_unlock(&mutex_sum);
    
    printf("Thread: %d, Finished\n", my_interval->tid);
    pthread_exit(NULL);
}

int main() {
    interval *intervals = malloc(sizeof *intervals * NUM_THREADS);
    pthread_t threads[NUM_THREADS];
    void *status;

    pthread_mutex_init(&mutex_sum, NULL);
    for(int i=0; i<NUM_THREADS; i++) {
        intervals[i].start = i*(NUMBERS/NUM_THREADS);
        intervals[i].end = (i+1) * (NUMBERS/NUM_THREADS) - 1;
        intervals[i].tid = i;
        pthread_create(&threads[i], NULL, add, (void *) &intervals[i]);
    }
    for(int i=0; i<NUM_THREADS; i++) 
        pthread_join(threads[i], &status);
    
    printf("Final global sum = ");
    print_uint128(sum);
    putchar('\n');
    free(intervals);
    exit(0);
}