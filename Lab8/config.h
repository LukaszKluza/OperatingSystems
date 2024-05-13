#ifndef OPERATINGSYSTEMS_CONFIG_H
#define OPERATINGSYSTEMS_CONFIG_H

#include <semaphore.h>
#include <stdbool.h>

#define MEMORY_NAME "memory_users_and_printers"
#define MAX_PRINTERS 32
#define BUFFER 124

typedef struct {
    char buffer[BUFFER];
    sem_t sem;
    bool status;
} printer;

typedef struct {
    printer printers[MAX_PRINTERS];
} memory_map;

#endif //OPERATINGSYSTEMS_CONFIG_H
