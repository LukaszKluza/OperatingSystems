#include <stdio.h>
#include "config.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

bool run = true;
Message message;
int socket_fd;
int id;
struct sockaddr_in server_addr;

void *receiving_routine(void *args);
void on_message();
void init(char *);
void to_all(char msg[MAX_MESSAGE_SIZE]);
void to_one(char msg[MAX_MESSAGE_SIZE], const int);
void list();
void stop();
void on_alive();

void handler(int signal){
    run = false;
    stop();
}

int main(int args, char* argv[]){
    if(args != 4){
        printf("Wrong number of arguments!\n");
        return -1;
    }

    char *client_name = argv[1];
    char *server_ip = argv[2];
    int port = atoi(argv[3]);

    if(strlen(client_name) > 20){
        printf("Too long name!\n");
        return -1;
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    struct sigaction action;
    action.sa_handler = handler;
    sigaction(SIGINT, &action, NULL);

    socket_fd = socket(AF_INET, SOCK_DGRAM,0);
    if(socket_fd == -1){
        printf("Socket error!\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    init(client_name);

    pthread_t receiving_thread;
    pthread_create(&receiving_thread, NULL, receiving_routine, (void *)&socket_fd);

    char type[32];
    while (run){
        printf("Enter the type: \n");
        scanf("%s", type);
        if(!strcmp(type, "2ALL")){
            char msg[MAX_MESSAGE_SIZE];
            printf("Enter the message: \n");
            scanf("%s", msg);
            to_all(msg);
        }
        if(!strcmp(type, "2ONE")){
            char msg[MAX_MESSAGE_SIZE];
            char to_id;
            printf("Enter the message: \n");
            scanf("%s", msg);
            printf("Enter the receiver ID: \n");
            scanf("%s", &to_id);
            to_one(msg, atoi(&to_id));
        }
        if(!strcmp(type, "LIST")){
            list();
        }
        if(!strcmp(type, "STOP")){
            stop();
        }
    }
    pthread_cancel(receiving_thread);
    close(socket_fd);
    return 0;
}

void *receiving_routine(void *args){
    while (run){
        socklen_t addr_len = sizeof(server_addr);
        if(recvfrom(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len) > 0){
            switch (message.type) {
                case STOP:
                    pthread_exit(NULL);
                case MESSAGE:
                    on_message();
                    break;
                case ALIVE:
                    on_alive();
                    break;
                default:
                    printf("[CLIENT] Unknown message type received\n");
                    break;
            }
        }
    }
    return NULL;
}

void on_message(){
    printf("[CLIENT] Message from: %d : %s\n", message.from, message.message);
}
void init(char *name){
    message.type = INIT;
    memcpy(message.message, name, MAX_NAME_SIZE);
    socklen_t addr_len = sizeof(server_addr);
    if(sendto(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("Sending error!\n");
    }else{
        printf("Init message sent\n");
    }
    if(recvfrom(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len) == -1){
        printf("Reading error!\n");
    }
    else{
        id = message.to;
        printf("[CLIENT] My ID: %d\n", id);
    }
}
void to_all(char msg[MAX_MESSAGE_SIZE]){
    if(strncpy(message.message,msg, MAX_MESSAGE_SIZE) == NULL){
        printf("Copy error!\n");
    }
    message.message[MAX_MESSAGE_SIZE-1] = '\0';
    message.type = TO_ALL;
    message.from = id;
    message.to = -1;
    printf("M: %s\n", message.message);
    if(sendto(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("Sending error!\n");
    } else{
        printf("[Client] Message sent\n");
    }
}

void to_one(char msg[MAX_MESSAGE_SIZE], const int to_id){
    if(strncpy(message.message,msg, MAX_MESSAGE_SIZE) == NULL){
        printf("Copy error!\n");
    }
    message.message[MAX_MESSAGE_SIZE-1] = '\0';
    message.type = TO_ONE;
    message.from = id;
    message.to = to_id;
    if(sendto(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("Sending error!\n");
    } else{
        printf("[Client] Message sent\n");
    }
}

void list(){
    message.type = LIST;
    message.to = -1;
    message.from = id;
    if(sendto(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("Sending error!\n");
    } else{
        printf("[Client] LIST sent\n");
    }
}

void stop(){
    message.type = STOP;
    message.to = -1;
    message.from = id;
    if(sendto(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("Sending error!\n");
    } else{
        printf("[Client] STOP sent\n");
    }
    run = false;
}

void on_alive(){
    printf("[CLIENT] Receive ALIVE message!\n");
    message.to = -1;
    message.from = id;
    message.type = ALIVE;
    if(sendto(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==-1){
        printf("Sending ALIVE response error!\n");
    }
    else{
        printf("[CLIENT] ALIVE response sent\n");
    }
}

