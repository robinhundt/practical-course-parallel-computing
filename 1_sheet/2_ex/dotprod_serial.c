#include <stdio.h>
#include <stdlib.h>

#define VECLEN 100000

typedef struct {
    double *a;
    double *b;
    double sum;
    int veclen;
} dotdata;

// why would anyone do this?!?!?
dotdata dotstr;

// compute dotprod of double vectors a and b reference in global struct dotstr
// and save result in dotstr.sum
void dotprod() {
    int start = 0;
    int end = dotstr.veclen;
    double *x = dotstr.a;
    double *y = dotstr.b;

    int sum = 0;
    for(int i=start; i<end; i++) 
        sum += x[i] * y[i];
    
    dotstr.sum = sum;
}

int main() {
    // allocate storafe for double vectors a and b of size VECLEN
    double *a = malloc(sizeof *a * VECLEN);
    double *b = malloc(sizeof *b * VECLEN);
    for(int i=0; i<VECLEN; i++) 
        a[i] = b[i] = 1;
    
    dotstr.veclen = VECLEN;
    dotstr.a = a;
    dotstr.b = b;
    dotstr.sum = 0;

    dotprod(); // this is terrible and should never be done

    printf("Sum = %f \n", dotstr.sum);

    //cleanup
    free(a);
    free(b);
    return 0;
}