#include <stdio.h>
#include <pthread.h>

void *answer(void *value) {
    // arguments to the start_routine specified at thread creation can only 
    // be of type void *, therefore cast to long is necessary
    printf("The answer is %ld.\n", (long) value);
    pthread_exit(NULL);
}

int main() {
    // obvious answer to everything
    long value = 42;
    pthread_t thread;
    // args for start_routine specified at thread creation can only
    // be of type void *
    pthread_create(&thread, NULL, answer, (void *) value);
    pthread_exit(0);
}
