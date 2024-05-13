#include <mqueue.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

struct my_message{
    long type;
    int id;
    char text[MAX_BUFFER_SIZE];
};

int findID(mqd_t *clients){
    for(int i =0; i<MAX_CLIENTS; ++i){
        if(clients[i] == -1){
            return i;
        }
    }
    return -1;
}

int main(){
    bool open = 1;
    int currClientsNum = 0;
    const char *name = "/server";
    struct my_message message;
    struct mq_attr attr = {
            .mq_flags = 0,
            .mq_maxmsg = 10,
            .mq_msgsize = sizeof(message)
    };

    mqd_t mqd = mq_open(name, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR, &attr);
    if(mqd == -1){
        printf("Error: mq_open() return -1\n");
        return 1;
    }

    mqd_t clients[MAX_CLIENTS];
    for(int i=0; i<MAX_CLIENTS; ++i){
        clients[i] = -1;
    }

    printf("START\n");

    while(open){
        mq_receive(mqd, (char *)&message, sizeof(message), NULL);
        if(message.type == 0){
            if(currClientsNum == MAX_CLIENTS){
                printf("ERROR: currClientsNum == MAX_CLIENTS\n");
                return 1;
            }
            currClientsNum++;
            int currID = findID(clients);
            if(currID == -1){
                printf("ERROR: currID == -1\n");
                return 1;
            }
            clients[currID] = mq_open(message.text, O_RDWR|O_CREAT, attr);
            struct my_message newMessage = {
                    .type = 1,
                    .id = currID
            };
            if(mq_send(clients[currID], (char * )&newMessage, sizeof(newMessage), 10)==-1){
                printf("ERROR: mq_send() == -1\n");
            }
            printf("INIT: %d, queueID: %d\n", currID, clients[currID]);

        }
        if(message.type == 1){
            printf("Message form: %d, %s\n", message.id, message.text);
            for(size_t i = 0; i<MAX_CLIENTS; ++i){
                if(clients[i] > -1 && i != message.id){
                    if(mq_send(clients[i], (char * )&message, sizeof(message), 10)==-1){
                        printf("ERROR: mq_send() == -1\n");
                    }
                }
            }
        }
        else if(message.type == -1){
            struct my_message new_message = {
                    .type = -1,
                    .id = message.id,
                    .text = "Clients closed"
            };
            if(mq_send(clients[message.id], (char * )&new_message, sizeof(new_message), 10)==-1){
                printf("ERROR: mq_send() == -1\n");
            }
            mq_close(clients[message.id]);
            clients[message.id] = -1;
            printf("Close: %d\n", message.id);
        }
        else if(message.type == -2){
            for(size_t i=0; i<MAX_CLIENTS; ++i){
                if(clients[i] > -1){
                    struct my_message newMessage = {
                            .type = -2,
                            .id = message.id,
                            .text = "Server exit"
                    };
                    if(mq_send(clients[i], (char * )&newMessage, sizeof(newMessage), 10)==-1){
                        printf("ERROR: mq_send() == -1\n");
                    }
                    mq_close(clients[i]);
                    clients[message.id] = -1;
                    printf("Close: %zu\n", i);
                }
            }
            printf("Exit server \n");
            open = false;
        }
    }

    if(mq_close(mqd)){
        printf("Error: mq_close(mqd)\n");
        return 1;
    }
    if(mq_unlink(name)){
        printf("Error mq_unlink(name)\n");
        return 1;
    }
    return  0;
}
