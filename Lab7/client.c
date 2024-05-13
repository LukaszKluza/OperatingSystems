#include <mqueue.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

struct my_message{
    long type;
    int id;
    char text[MAX_BUFFER_SIZE];
};

char my_name[MAX_BUFFER_SIZE];
const char *server_name = "/server";
mqd_t client_mqd;
mqd_t server_mqd;

void handler(int signal){
    printf("SIGNAL: %d\n", signal);
    mq_close(client_mqd);
    mq_close(server_mqd);
    mq_unlink(my_name);
    mq_unlink(server_name);
    exit(EXIT_SUCCESS);
}

int main(){
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    pid_t my_pid = getpid();
    bool work = true;
    int my_id;
    snprintf(my_name, sizeof(my_name), "/client%d", my_pid);
    struct my_message message ={
            .type = 0,
    };
    memcpy(message.text, my_name, sizeof(my_name));
    struct my_message receive_message;
    struct mq_attr attr = {
            .mq_flags = 0,
            .mq_maxmsg = 10,
            .mq_msgsize = sizeof(message)
    };
    printf("My name: %s, server name: %s\n", my_name, server_name);
    client_mqd = mq_open(my_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);
    server_mqd = mq_open(server_name, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR, NULL);
    if(mq_send(server_mqd, (char *)&message, sizeof(message), 10)==-1){
        printf("ERROR_1\n");
    }
    if(mq_receive(client_mqd, (char *)&receive_message, sizeof(receive_message), NULL)==-1){
        printf("ERROR_2\n");
    }
    my_id = receive_message.id;
    printf("My id: %d, my pid: %d\n", my_id, my_pid);
    pid_t child = fork();
    if(child == -1){
        printf("ERROR\n");
        return 1;
    }
    char type[MAX_BUFFER_SIZE];
    char message_[MAX_BUFFER_SIZE];
    if(child == 0){
        while (work){
            mq_receive(client_mqd, (char *)&receive_message, sizeof(receive_message), NULL);
            if(receive_message.type == 1){
                printf("Message from: %d, %s\n", receive_message.id, receive_message.text);
            }
            else if (receive_message.type < 0) {
                printf("Message from: %s\n", receive_message.text);
                kill(getppid(),SIGUSR1);
                work = 0;
            }
        }
    }else{
        while(work){
            scanf("%s", type);
            if(!strcmp(type, "1")){
                struct my_message new_message = {
                        .type = 1,
                        .id = my_id
                };
                printf("Enter the message: ");
                scanf("%s", message_);
                memcpy(new_message.text, message_, sizeof(message_));
                mq_send(server_mqd, (char *)&new_message, sizeof(new_message), 10);
            }
            else if(!strcmp(type, "-1") || !strcmp(type, "-2")){
                struct my_message new_message = {
                        .type = atoi(type),
                        .id = my_id
                };
                mq_send(server_mqd, (char *)&new_message, sizeof(new_message), 10);
            }
        }
    }
    return  0;
}
