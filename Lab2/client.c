#include <stdio.h>

extern int test_collatz_convergence(int input, int max_iter);

int main(){
    const int MAX_ITER = 50;
    int numbers[] = {12, 14,19,22,57,129};
    int numbers_size = sizeof(numbers)/sizeof(numbers[0]);

    for(int i=0; i<numbers_size; ++i){
        printf("%d: %d\n", numbers[i], test_collatz_convergence(numbers[i],MAX_ITER));
    }
    return 0;
}