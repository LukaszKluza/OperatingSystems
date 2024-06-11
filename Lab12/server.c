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
    struct sockaddr_in addr;
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
struct sockaddr_in server_addr;

void *add_new_clients(void *);
void *ping_clients(void *);
void init(const int, struct sockaddr_in *);
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

    socket_fd = socket(AF_INET, SOCK_DGRAM,0);
    if(socket_fd == -1){
        printf("Socket error!\n");
        return -1;
    }

    pthread_create(&adding_thread, NULL, add_new_clients, NULL);
    pthread_create(&alive_thread, NULL, ping_clients,NULL);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }
    if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        printf("Bind error!\n");
        return -1;
    }

    while (run) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int recv_len = recvfrom(socket_fd, &message, sizeof(message), 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len == -1) {
            printf("Receiving error!\n");
            continue;
        }

        int client_id = -1;
        for (int i = 0; i < MAX_CLIENTS_NUMBER; ++i) {
            if (!clients[i].empty && clients[i].addr.sin_addr.s_addr == client_addr.sin_addr.s_addr && clients[i].addr.sin_port == client_addr.sin_port) {
                client_id = clients[i].id;
                break;
            }
        }

        if (message.type == INIT) {
            init(client_id, &client_addr);
        } else if (client_id != -1) {
            if (message.type == TO_ALL) {
                printf("[Server] Message received from %d to ALL, %s\n", client_id, message.message);
                to_all(client_id, message.message);
            }
            if (message.type == TO_ONE) {
                printf("[Server] Message received from %d to %d, %s\n", client_id, message.to, message.message);
                to_one(message.from, message.to, message.message);
            }
            if (message.type == LIST) {
                printf("[Server] List operation received from %d\n", client_id);
                list();
            }
            if (message.type == STOP) {
                printf("[Server] Stop operation received from %d\n", client_id);
                on_stop(client_id);
            }
            if (message.type == ALIVE) {
                printf("[Server] Alive operation received from %d\n", client_id);
                clients[client_id].last_alive = time(NULL);
            }
        } else {
            printf("[Server] Unknown client.\n");
        }
    }


    for(int i = 0; i < MAX_CLIENTS_NUMBER; ++i){
        if(!clients[i].empty){
            clients[i].empty = true;
        }
    }
    close(socket_fd);
    return 0;
}


void *ping_clients(void *args){
    while (run){
        for(int i = 0; i < MAX_CLIENTS_NUMBER; ++i){
            if(!clients[i].empty){
                message.type = ALIVE;
                if (sendto(socket_fd, &message, MESSAGE_SIZE, 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr)) == -1) {
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

void init(const int client_socket_fd, struct sockaddr_in *client_addr){
    printf("[Server] Received INIT message\n");
    int client_id = find_ID();
    if(client_id == -1){
        printf("[Server] Too many clients, max number of clients: %d\n", MAX_CLIENTS_NUMBER);
        return;
    }
    clients[client_id].empty = false;
    clients[client_id].id = client_id;
    clients[client_id].addr = *client_addr;
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
    if (sendto(socket_fd, &msg, MESSAGE_SIZE, 0, (struct sockaddr *)&clients[to_id].addr, sizeof(clients[to_id].addr)) == -1) {
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
    clients[client_id].empty = true;
    clients_number--;
    memset(clients[client_id].name, 0, MAX_NAME_SIZE);
}

