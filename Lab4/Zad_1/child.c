#include <stdio.h>
#include <unistd.h>

int main(){
    int child_pid, parent_pid;
    child_pid = (int)getpid();
    parent_pid = (int)getppid();

    printf("My PID: %d, my parent's PID: %d\n", child_pid, parent_pid);

    return 0;
}
