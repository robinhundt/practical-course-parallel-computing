#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 8

// struct to hold data passed to thread start_routine
typedef struct {
    int thread_id;
    int sum;
    char *message;
} thread_data;

// start_routine passed to pthread_create
void *print_hello(void *threadarg) {
    sleep(1);
    thread_data *my_data = (thread_data *) threadarg;
    int taskid = my_data->thread_id;
    int sum = my_data->sum;
    char *hello_msg = my_data->message;
    printf("Thread %d: %s  Sum=%d\n", taskid, hello_msg, sum);
    pthread_exit(NULL);
}

int main() {
    char *messages[NUM_THREADS];
    thread_data thread_data_array[NUM_THREADS];

    pthread_t threads[NUM_THREADS];
    messages [0] = "English: Hello World!";
    messages [1] = "French: Bonjour, le monde!";
    messages [2] = "Spanish: Hola al mundo";
    messages [3] = "Klingon: Nuq neH!";
    messages [4] = "German: Guten Tag, Welt!";
    messages [5] = "Russian: Zdravstvytye, mir!";
    messages [6] = "Japan: Sekai e konnichiwa!";
    messages [7] = "Latin: Orbis, te saluto!";

    int sum = 0;
    for(int t=0; t<NUM_THREADS; t++) {
        sum += t;
        // initialize thread_data struct in thread_data_array  
        thread_data_array[t].thread_id = t;
        thread_data_array[t].sum = sum;        
        thread_data_array[t].message = messages[t];
        printf("Creating thread %d\n", t);
        // cast pointer to thread_data at pos t to (void *) in order to pass it to pthread_create
        int rc = pthread_create(&threads[t], NULL, print_hello, (void *) &thread_data_array[t]);
        // pthread_create returns 0 if thread creation was unsuccessful
        if(rc) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            // calling exit will terminate all threads
            exit(-1);
        }
    }
    // main thread will wait for termination of other threads
    pthread_exit(NULL);
}
