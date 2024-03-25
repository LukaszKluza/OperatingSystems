#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("ERROR\n");
        return -1;
    }
    int num_processes = atoi(argv[1]);
    pid_t child_pid;
    for(int i=0; i<num_processes; ++i){
        child_pid = fork();
        if(child_pid == 0){
            char *args [] = {"./child", NULL};
            execvp(args[0], args);
            exit(0);
        }
    }
    for (int i = 0; i < num_processes; ++i) {
        wait(NULL);
    }
    printf("Argv[1] = %d\n", num_processes);
    return 0;
}
