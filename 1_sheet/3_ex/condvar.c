#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

int count = 0;
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

void *inc_count(void *t) {
    long my_id = (long) t;
    
    for(int i=0; i<TCOUNT; i++) {
        pthread_mutex_lock(&count_mutex);
        count++;
        if(count == COUNT_LIMIT) {
            printf("inc_count(): thread: %ld, count = %d. Threshold reached.\n", my_id, count);
            pthread_cond_signal(&count_threshold_cv);
            printf("inc_count(): thread %ld, count = %d. Just sent signal.\n", my_id, count);
        }
        printf("inc_count(): thread %ld, count = %d. Unlocking mutex.\n", my_id, count);
        pthread_mutex_unlock(&count_mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *watch_count(void *t) {
    long my_id = (long) t;
    printf("Starting watch_count(): thread %ld\n", my_id);
    pthread_mutex_lock(&count_mutex);
    while(count < COUNT_LIMIT) {
        printf("watch_count(): thread: %ld, count = %d. Going into wait...\n", my_id, count);
        pthread_cond_wait(&count_threshold_cv, &count_mutex);
        printf("watch_count(): thread: %ld, count = %d. Condition signal received.\n", my_id, count);    
        printf("watch_count(): thread: %ld, count = %d. Updating the value of count...\n", my_id, count);            
        count += 125;
    }
    printf("watch_count(): thread: %ld, count = %d. Unlocking mutex.\n", my_id, count);            
    pthread_mutex_unlock(&count_mutex);
    pthread_exit(NULL);
}

int main() {
    void *(*func_ptrs[NUM_THREADS])(void *) = {watch_count, inc_count, inc_count};
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&count_mutex, NULL);
    pthread_cond_init(&count_threshold_cv, NULL);
    for(long t=0; t<NUM_THREADS; t++)
        pthread_create(&threads[t], NULL, func_ptrs[t], (void *) t);

    for(int i=0; i<NUM_THREADS; i++) 
        pthread_join(threads[i], NULL);
    printf("Main(): Waited and joined with %d threads. Final value of count = %d.", NUM_THREADS, count);
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_threshold_cv);
    exit(0);
}