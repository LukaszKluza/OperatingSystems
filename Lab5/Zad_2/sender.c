#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>


int main(int argc, char *argv[]){
    if(argc < 3){
        printf("ERROR: To few arguments\n");
        return 1;
    }
    int catcherPid = atoi(argv[1]);
    int  mode = atoi(argv[2]);
    union sigval modeValue = {mode};
    sigqueue(catcherPid, SIGUSR1, modeValue);
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigsuspend(&mask);

    return 0;
}