#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main () 
{
  int   i, n;
  float a[100], b[100], sum; 

  /* Some initializations */
  n = 100;
  for (i=0; i < n; i++)
    a[i] = b[i] = i * 1.0;
  sum = 0.0;

  #pragma omp parallel shared(sum, a, b) private(i)
  {
    printf("Hello from thread: %d\n", omp_get_thread_num());
    #pragma omp for schedule(guided) reduction(+:sum) nowait
    for (i=0; i < n; i++)
      sum = sum + (a[i] * b[i]);
  }

  printf("Sum = %f\n",sum);

  return 0;
}
