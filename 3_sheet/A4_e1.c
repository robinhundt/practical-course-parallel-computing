#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double myFunc();
//Execute as: ./e1 6   (here 6 is command line param for the size of the vectors to be multiplied)
int main(int argc, const char *argv[])
{
  double tmp;

  //Snippet#1: What is wrong with the following code? Please fix
  // Parallelization is useless: 
  // http://pubs.opengroup.org/onlinepubs/9699919799/functions/flockfile.html
  //> All functions that reference (FILE *) objects, except those with names 
  //> ending in _unlocked, shall behave as if they use flockfile() and 
  //> funlockfile() internally to obtain ownership of these (FILE *) objects.
  // #pragma omp parallel for
  printf("Snippet#1\n");
  for (int i = 0; i < 20; i++)
  {
    printf("%d\n", i);
  }

  //
  //Snippet#2: What is wrong with the following code? Please fix
  //
  printf("Snippet#2\n");
  #pragma omp parallel default(none) private(tmp) num_threads(2)
  {
    // N code lines
    #pragma omp for      // parallel without enclosing for loop doesn't make sense...
    for (int i = 0; i < 10; i++)
    {
      tmp = myFunc();
    }
  }
  //
  //Snippet#3: What is wrong with the following code? Please fix
  //
  printf("Snippet#3\n");
  #pragma omp parallel default(none) private(tmp) num_threads(4)
  {
    // omp_set_num_threads(2); setting num_threads within parallel region
    // will have no effect
    #pragma omp for
    for (int i = 0; i < 10; i++)
    {
      tmp = myFunc();
      printf("tmp: %f\n", tmp);
    }
  }
  //
  //Snippet#4: What is wrong with the following code? Please fix
  //
  printf("Snippet#4\n");
  int tmp_reduce = 0;
  int i = 1;
  #pragma omp parallel default(none) shared(i) reduction(+: tmp_reduce)
  {
    tmp_reduce += i;
    #pragma omp master
    printf("Num threads: %d\n", omp_get_num_threads()); // will be the same for every thread in the team
  }
  printf("tmp_reduce: %d\n", tmp_reduce);
  return 0;
}

double myFunc()
{
  double i = 0;  // curiously i was implicitly set to 0 before as well
  i++;
  return i; // was missing a return but still useless function
}
