#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int status = -1;
int counter = 0;
void signalHandler(int signalNum, siginfo_t *info, void *context){
    status = info->si_value.sival_int;
    counter++;
    kill(info->si_pid, SIGUSR1);
}

int main(){

    printf("Catcher PID: %d\n", getpid());
    struct sigaction action;
    action.sa_sigaction = &signalHandler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);
    while(1){
        switch (status) {
            case 1:
                for(int i=0; i<=100; ++i){
                    printf("%d, ", i);
                }
                printf("\n");
                status = -1;
                break;
            case 2:
                printf("Number of changes %d\n", counter);
                status = -1;
                break;
            case 3:
                return 0;
            default:
                break;
        }
    }
}