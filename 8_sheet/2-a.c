#include <stdio.h>
#include <stdlib.h>

int is_power_of_two(int x)
{
    // https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2/600306#600306
    return (x != 0) && ((x & (x - 1)) == 0);
}

double sum_arr(float arr[], int n) {
    double sum = 0;
    for(int i=0; i<n; i++)
        sum += arr[i];
    return sum;
}

int main(int argc, char const *argv[])
{
    const int N = argc-1;    
    if(N == 0 || !is_power_of_two(N)) {
        printf("Please call the program with the 2^N array elements "
                "to be summed as arguments.\n");
        return 1;
    }
    float arr[N];
    for(int i=0; i<N; i++) {
        arr[i] = atof(argv[i+1]);
    }
    printf("Result is: %f\n", sum_arr(arr, N));
    return 0;
}
