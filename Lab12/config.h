#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CLIENTS_NUMBER 8
#define MAX_MESSAGE_SIZE 1024
#define PING_TIME 30

typedef enum MessageType{
    INIT = 1,
    LIST = 2,
    TO_ALL = 3,
    TO_ONE = 4,
    STOP = 5,
    ALIVE = 6,
    MESSAGE = 7
} MessageType;

typedef struct Message{
    MessageType type;
    int from;
    int to;
    char message[MAX_MESSAGE_SIZE];
} Message;

#define MESSAGE_SIZE sizeof(Message)
#define MAX_NAME_SIZE sizeof(char)*20
#endif //CONFIG_H
