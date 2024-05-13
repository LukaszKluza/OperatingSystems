#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <memory.h>
#include <time.h>
#include "config.h"

bool work = true;

void handler(){
    work = false;
}

void messageGenerator(char *buffer, int length){
    for(size_t i=0; i<length; ++i){
        buffer[i] = 'a' + rand() % 26;
    }
    buffer[length] = '\0';
}

size_t findPrinter(size_t usersNumber, memory_map *mem_map){
    int value;
    for(size_t j = 0; j < usersNumber; ++j){
        sem_getvalue(&mem_map->printers[j].sem, &value);
        if(value > 0){
            return j;
        }
    }
    return MAX_PRINTERS;
}


int main(int argc, char *argv[]){
    if(argc != 2){
        printf("ERROR: Wrong numbers of arguments!\n");
        return -1;
    }
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);

    char buffer[BUFFER];
    int usersNumber = atoi(argv[1]);
    int mem_fd = shm_open(MEMORY_NAME, O_CREAT |O_RDWR,0666);
    if(mem_fd == -1){
        printf("ERROR: shm_open() error!\n");
        return EXIT_FAILURE;
    }
    memory_map *mem_map = mmap(NULL, sizeof(memory_map), PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    if(mem_map == MAP_FAILED){
        printf("ERROR: mmap() error!\n");
        return EXIT_FAILURE;
    }
    for(size_t i = 0; i<usersNumber; ++i){
        pid_t new_user = fork();
        if(new_user == -1){
            printf("ERROR: fork() error!\n");
            return EXIT_FAILURE;
        } else if(new_user == 0){
            srand(time(NULL)+getpid());
            while (work){
                messageGenerator(buffer, 10);
                size_t freePrinter = findPrinter(usersNumber,mem_map);

                while(freePrinter == MAX_PRINTERS){
                    freePrinter = findPrinter(usersNumber,mem_map);
                }
                if(sem_wait(&mem_map->printers[freePrinter].sem) == -1){
                    printf("ERROR: sem_wait() error\n");
                }
                memcpy(mem_map->printers[freePrinter].buffer, buffer, 10 * sizeof(char));
                mem_map->printers[freePrinter].status = 1;
                printf("Message user's %zu is printing by printer %zu\n", i, freePrinter);
                sleep(1);
            }
            exit(EXIT_SUCCESS);
        }
    }
    while(wait(NULL) > 0) {};

    if(munmap(mem_map, sizeof(memory_map)) <0){
        printf("ERROR: munmap()\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
