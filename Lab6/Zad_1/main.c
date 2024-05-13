#include <printf.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

long double f(long double x){
    return 4/(x*x + 1);
}

long double integral(long double start, long double (*fun)(long double), long double h, unsigned long long n){

    long double res = 0;
    for(unsigned  long long i = 0; i < n; ++i){
        res += fun(start+i*h);
    }

    return res * h;
}

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("Too few arguments");
        return -1;
    }

    long double h = strtold(argv[1], NULL);
    long long n = strtoll(argv[2], NULL, 10);


    if(1/h < n) {
        printf("Too much processes\n");
        return -1;
    }

    FILE *timeFile = fopen("time_zad_1.txt", "a");
    double time_start, time_dif;

    unsigned long long integral_per_process = 1/(n*h);
    long double result =0, res_dif=0;
    long double start;


    int fd[n][2];
    time_start =(double)clock()/ CLOCKS_PER_SEC;
    for(int i=0; i<n; ++i) {
        start = i * h*integral_per_process;
        if(i == n-1){
            integral_per_process = (1-start)/h;
        }
        if(pipe(fd[i])<0){
            return -1;
        }
        pid_t pid = fork();

        if (pid == 0) {
            close(fd[i][0]);
            long double temp_res = integral(i * h*integral_per_process, f,h, integral_per_process);
            if(write(fd[i][1], &temp_res, sizeof(long double))<0){
                return -1;
            }
            close(fd[i][1]);
            exit(0);
        }else{
            close(fd[i][1]);
            long double temp_res = 0;
            read(fd[i][0], &temp_res, sizeof(long double));
            result += temp_res;
            close(fd[i][0]);
        }
    }
    time_dif = (((double)clock())/CLOCKS_PER_SEC) - time_start;
    res_dif = fabsl(M_PI - result);
    fprintf(timeFile, "Total time for h= %Lf, n=%Ld: %f, result=  %Lf, dif: %Lf\n", h,n, time_dif, result, res_dif);
    printf("Result: %Lf\n",result);
    return 0;
}