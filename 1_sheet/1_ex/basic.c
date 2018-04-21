#include <stdio.h>
#include <pthread.h>

void *hello() {
    printf("Hello World.\n");
    // terminate the thread where `hello` is executed
    pthread_exit(NULL);
}


int main() {

    // identifier for the thread, typedef of unsigned long
    pthread_t thread;
    // 1. first param is reference to handler variable
    // 2. attr param. is null, so defaults are used
    // 3. hello is the start routine which is called by the thread
    // 4. no args are passed to the function executing in the thread
    pthread_create(&thread, NULL, hello, NULL);
    // calling `pthread_exit` rather than `exit` allows other threads to continue until they exit
    // when all threads have exited, `exit` is called
    pthread_exit(0);
}