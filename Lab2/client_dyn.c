#include <stdio.h>
#ifdef DYNAMIC
    #include <dlfcn.h>
#endif

int main(void){
#ifdef DYNAMIC
    void *handle= dlopen("./libcollatz.so", RTLD_LAZY);
    if(!handle){
        printf("Opening error\n");
        return 0;
    }
    int (*test_collatz_convergence)(int input, int max_iter);
    test_collatz_convergence = dlsym(handle, "test_collatz_convergence");

    if(dlerror()){
        printf("Collatz error\n");
        return 0;
    }

    const int MAX_ITER = 50;
    int numbers[] = {12, 14,19,22,57,129};
    int numbers_size = sizeof(numbers)/sizeof(numbers[0]);

    for(int i=0; i<numbers_size; ++i){
        printf("%d: %d\n", numbers[i], test_collatz_convergence(numbers[i],MAX_ITER));
    }
    dlclose(handle);
#endif
    printf("Symbol preprocesora DYNAMIC nie został zdefiniowany.\n");
    return 0;
}