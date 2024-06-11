#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

void signalHandler(int signalNumber){
    printf("Signal: %d\n", signalNumber);
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("ERROR\n");
        return 1;
    }

    if(!strcmp("ignore", argv[1])){
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
    }else if(!strcmp("handler", argv[1])){
        signal(SIGUSR1, signalHandler);
        raise(SIGUSR1);
    }else if(!strcmp("mask", argv[1])){
        sigset_t newMask;
        sigemptyset(&newMask);
        sigaddset(&newMask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &newMask, NULL);
    } else{
        signal(SIGUSR1, SIG_DFL);
        raise(SIGUSR1);
    }

    return 0;
}