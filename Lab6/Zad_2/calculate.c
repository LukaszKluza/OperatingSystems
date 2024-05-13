#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "config.h"

#define epsilon 1e-10

long double f(long double x){
    return 4/(x*x + 1);
}

long double integral(config_s config, long double (*fun)(long double)){

    long double start = config.start;
    long double res = 0;
    while (start < config.end){
        res+=fun(start) * config.h;
        start += config.h;
    }
    return res;
}

int main() {

    const char *in_path = "in_pipe";
    const char *out_path = "out_pipe";
    mkfifo(in_path, 0666);
    mkfifo(out_path, 0666);
    int fd_read = open(in_path, O_RDONLY);
    int fd_write = open(out_path, O_WRONLY);
    config_s config;
    long double result;
    while (1){
        if (read(fd_read, &config, sizeof(config)) ==  sizeof(config)){
            result = integral(config, f);
            if (write(fd_write, &result, sizeof(result))< 0){
                printf("WRITE ERROR\n");
            }
        }
    }
}
