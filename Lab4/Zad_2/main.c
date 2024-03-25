#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("ERROR\n");
    }
    printf("Name: %s\n", argv[0]);
    int local = 0;
    pid_t child_pid;
    child_pid = fork();
    if(child_pid == -1){
        perror("fork");
        return 1;
    }
    if(child_pid == 0){
        global++;
        local++;
        printf("child process\n");
        printf("child pid = %d, parent pid = %d\n",getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        execl("/bin/ls", "ls", argv[1], NULL);
        perror("execl");
        exit(1);
    }
    int status;
    waitpid(child_pid, &status, 0);
    printf("parent process\n");
    printf("parent pid = %d, child pid = %d\n",getpid(), child_pid);

    if (WIFEXITED(status)) {
        printf("Child exit code: %d\n", WEXITSTATUS(status));
    } else {
        printf("Child process terminated abnormally\n");
    }

    printf("Parent's local = %d, child's global = %d\n", local, global);
    return status;
}