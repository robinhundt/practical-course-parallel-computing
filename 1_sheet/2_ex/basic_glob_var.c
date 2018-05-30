#include <stdio.h>
#include <pthread.h>

int answer = 42;

// print value of global variable answer 
void *hello() {
    printf("The answer is again %d\n", answer);
    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    // create two threads with default parameters and pass `hello()` as start routine
    pthread_create(&thread, NULL, hello, NULL);
    pthread_create(&thread, NULL, hello, NULL);
    pthread_exit(0);
}