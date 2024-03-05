#include <stdio.h>
#include <dlfcn.h>


int main(void){
    void *handle= dlopen("./libcollatz.so", RTLD_LAZY);
    if(!handle){
        printf("Opening error\n");
        return 0;
    }
    int (*collatz)(int input, int max_iter);
    collatz = dlsym(handle, "test_collatz_convergence");

    if(dlerror()){
        printf("Collatz error\n");
        return 0;
    }

    const int MAX_ITER = 50;
    int numbers[] = {12, 14,19,22,57,129};
    int numbers_size = sizeof(numbers)/sizeof(numbers[0]);

    for(int i=0; i<numbers_size; ++i){
        printf("%d: %d\n", numbers[i], collatz(numbers[i],MAX_ITER));
    }
    dlclose(handle);
    return 0;
}