#include <stdio.h>
#include "config.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <poll.h>
#include <pthread.h>
#include <time.h>

typedef struct client{
    bool empty;
    int id;
    int socket_fd;
    char name[20];
    time_t last_alive;
}client;

client clients[MAX_CLIENTS_NUMBER];
struct pollfd poll_fds[MAX_CLIENTS_NUMBER];
int clients_number = 0;
bool run = true;
int socket_fd;
Message message;
pthread_t adding_thread, alive_thread;

void *add_new_clients(void *);
void *ping_clients(void *);
void init(const int);
void to_all(const int, char text[MAX_MESSAGE_SIZE]);
void to_one(const int, const int, char text[MAX_MESSAGE_SIZE]);
void list();
void on_stop(const int);
int find_ID();
void check_clients_alive();

void handler() {
    run = false;
}

int main(int args, char* argv[]){
    if(args != 3){
        printf("Wrong number of arguments\n");
        return -1;
    }
    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    setvbuf(stdout, NULL, _IONBF, 0);

    struct sigaction action;
    action.sa_handler = handler;
    sigaction(SIGINT, &action, NULL);

    for(int i = 0; i<MAX_CLIENTS_NUMBER; ++i){
        clients[i].empty = true;
    }

    socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd == -1){
        printf("Socket error!\n");
        return -1;
    }

    pthread_create(&adding_thread, NULL, add_new_clients, NULL);
    pthread_create(&alive_thread, NULL, ping_clients,NULL);

    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &socket_address.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }
    if(bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1){
        printf("Bind error!\n");
        return -1;
    }
    if(listen(socket_fd, MAX_CLIENTS_NUMBER) == -1){
        printf("Listen error!\n");
        return -1;
    }

    while (run){
        poll(poll_fds, MAX_CLIENTS_NUMBER, 0);
        for(int i=0; i<MAX_CLIENTS_NUMBER; ++i){
            if(!clients[i].empty && (poll_fds[i].revents & POLLIN)){
                int client_id = clients[i].id;
                if(recv(clients[i].socket_fd, &message, sizeof(message),0)==-1){
                    printf("Receiving error!\n");
                }
                if(message.type == TO_ALL) {
                    printf("[Server] Message received from %d to ALL, %s\n", client_id, message.message);
                    to_all(client_id, message.message);
                }
                if(message.type == TO_ONE){
                    printf("[Server] Message received from %d to %d, %s\n", client_id, message.to, message.message);
                    to_one(message.from, message.to, message.message);
                }
                if(message.type == LIST){
                    printf("[Server] List operation received from %d\n", client_id);
                    list();
                }
                if(message.type == STOP){
                    printf("[Server] Stop operation received from %d\n", client_id);
                    on_stop(client_id);
                }
                if(message.type == ALIVE){
                    printf("[Server] Alive operation received from %d\n", client_id);
                    clients[client_id].last_alive = time(NULL);
                }
                poll_fds[i].revents = 0;
            }
        }
    }

    for(int i = 0; i < MAX_CLIENTS_NUMBER; ++i){
        if(!clients[i].empty){
            close(clients[i].socket_fd);
            clients[i].empty = true;
        }
    }
    close(socket_fd);
    return 0;
}

void *add_new_clients(void *args){
    while (run){
        int client_socket_fd = accept(socket_fd, NULL, NULL);
        if(client_socket_fd == -1){
            printf("Accept error!\n");
            continue;
        }
        printf("Server added new client socket: %d\n", client_socket_fd);
        if(read(client_socket_fd, &message, MESSAGE_SIZE) == -1){
            printf("Receive error\n");
            continue;
        }else {
            printf("Succeeded\n");
            printf("[Server] Received bytes: %lu\n", sizeof(message));
        }
        MessageType message_type = message.type;
        if (message_type == INIT) {
            init(client_socket_fd);
        }
        else {
            printf("[Server] Invalid message type (%d)", message_type);
            continue;
        }
    }
    return NULL;
}

void *ping_clients(void *args){
    while (run){
        for(int i = 0; i < MAX_CLIENTS_NUMBER; ++i){
            if(!clients[i].empty){
                message.type = ALIVE;
                if(send(clients[i].socket_fd, &message, MESSAGE_SIZE, 0) == -1){
                    printf("Error during sending ALIVE message to client %d!\n", clients[i].id);
                }
            }
        }
        sleep(PING_TIME);
        check_clients_alive();
    }
    return NULL;
}


void check_clients_alive(){
    time_t now = time(NULL);
    for(int i = 0; i < MAX_CLIENTS_NUMBER; ++i){
        if(!clients[i].empty && clients[i].last_alive != 0){
            if(now - clients[i].last_alive > 2*PING_TIME){
                printf("[Server] Client %d did not respond to ALIVE message. Removing client.\n", clients[i].id);
                on_stop(clients[i].id);
            } else{
                printf("[Server] Client %d respond to ALIVE message.\n", clients[i].id);
            }
        }
    }
}

void init(const int client_socket_fd){
    printf("[Server] Received INIT message\n");
    int client_id = find_ID();
    if(client_id == -1){
        printf("[Server] Too many clients, max number of clients: %d\n", MAX_CLIENTS_NUMBER);
        return;
    }
    clients[client_id].empty = false;
    clients[client_id].id = client_id;
    clients[client_id].socket_fd = client_socket_fd;
    memcpy(clients[client_id].name, message.message, sizeof(clients[client_id].name));

    poll_fds[client_id].fd = client_socket_fd;
    poll_fds[client_id].events = POLLIN | POLLOUT;

    clients_number++;

    message.type = INIT;
    message.from = -1;
    message.to = client_id;
    if (send(client_socket_fd, &message, MESSAGE_SIZE, 0) == -1) {
        printf("Error during adding new client with id: %d\n", client_id);
    }
    else {
        printf("[Server] Added new client with ID: %d\n", client_id);
    }
}

int find_ID(){
    for(int i = 0; i < MAX_CLIENTS_NUMBER; ++i){
        if(clients[i].empty) return i;
    }
    return -1;
}
void to_all(const int client_id, char text[MAX_MESSAGE_SIZE]){
    for(int i=0; i<MAX_CLIENTS_NUMBER; ++i){
        if(!clients[i].empty && clients[i].id != client_id){
            to_one(client_id, clients[i].id, text);
        }
    }
}
void to_one(const int from_id, const int to_id, char text[MAX_MESSAGE_SIZE]){
    Message msg;
    msg.from = from_id;
    msg.to = to_id;
    msg.type = MESSAGE;
    strcpy(msg.message, text);
    if(send(clients[to_id].socket_fd, &msg, MESSAGE_SIZE,0) == -1){
        printf("Error during sending message from %d to %d!\n", from_id, to_id);
    } else{
        printf("[Server] Message sent from  %d to %d\n", from_id, to_id);
    }
}

void list(){
    char text[MAX_MESSAGE_SIZE];
    text[0] = '\0';
    for(int i = 0; i < MAX_CLIENTS_NUMBER; ++i){
        if(!clients[i].empty){
            char buffer[20];
            snprintf(buffer, sizeof (buffer),"%d", clients[i].id);
            strncat(text, "\n", sizeof(text) - strlen(text) - 1);
            strncat(text, clients[i].name, sizeof(text) - strlen(text) - 1);
            strncat(text, " ", sizeof(text) - strlen(text) - 1);
            strncat(text, buffer, sizeof(text) - strlen(text) - 1);
        }
    }
    to_all(-1, text);
}

void on_stop(int client_id){
    close(clients[client_id].socket_fd);
    clients[client_id].empty = true;
    poll_fds[client_id].fd = -1;
    poll_fds[client_id].events = 0;
    clients_number--;
    memset(clients[client_id].name, 0, MAX_NAME_SIZE);
}

