#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "config.h"

int main(){

    const char *in_path = "in_pipe";
    const char *out_path = "out_pipe";
    int fd_write = open(in_path, O_WRONLY);
    int fd_read = open(out_path, O_RDONLY);
    config_s config;
    long double result;
    while (1){
        printf("Start: ");
        scanf("%Lf", &config.start);

        printf("End: ");
        scanf("%Lf", &config.end);

        printf("h: ");
        scanf("%Lf", &config.h);

        if (write(fd_write, &config, sizeof(config)) < 0){
            printf("ERROR WRITING\n");
            return -1;
        }
        if (read(fd_read, &result, sizeof(long double)) < 0){
            printf("ERROR READING\n");
            return -1;
        }
        printf("Start: %Lf End: %Lf H: %Lf Result: %Lf\n", config.start, config.end, config.h, result);
    }
}
