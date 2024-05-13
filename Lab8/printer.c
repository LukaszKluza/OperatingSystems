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
#include "config.h"

bool work = true;

void handler(){
    work = false;
}


int main(int argc, char *argv[]){
    if(argc != 2){
        printf("ERROR: Wrong numbers of printers!\n");
        return EXIT_FAILURE;
    }
    int printersNumber = atoi(argv[1]);
    if(printersNumber <= 0 || printersNumber > MAX_PRINTERS){
        printf("ERROR: To much printers!\n");
        return EXIT_FAILURE;
    }
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    int mem_fd = shm_open(MEMORY_NAME, O_CREAT | O_RDWR,0666);
    if(mem_fd == -1){
        printf("ERROR: shm_open() error!\n");
        return EXIT_FAILURE;
    }
    if(ftruncate(mem_fd, sizeof (memory_map))==-1){
        printf("Error: ftruncate() error\n");
        return EXIT_FAILURE;
    }
    memory_map *memory_map = mmap(NULL, sizeof(memory_map), PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    if(memory_map == MAP_FAILED){
        printf("ERROR: mmap() error!\n");
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < printersNumber; i++){
        sem_init(&memory_map->printers[i].sem, 1, 1);

        pid_t printer_pid = fork();
        if(printer_pid == -1) {
            printf("ERROR: fork() error\n");
            return EXIT_FAILURE;
        }
        else if(printer_pid == 0) {
            while(work) {
                if (memory_map->printers[i].status == 1) {

                    size_t idx = 0;
                    while (memory_map->printers[i].buffer[idx] != '\0'){
                        printf("Printer: %zu %c\n", i, memory_map->printers[i].buffer[idx++]);
                        usleep(500000);
                    }

                    memory_map->printers[i].status = 0;
                    sem_post(&memory_map->printers[i].sem);
                }
            }
            exit(EXIT_SUCCESS);
        }
    }
    while(wait(NULL) > 0) {}

    for (size_t i = 0; i < printersNumber; ++i)
        if(sem_destroy(&memory_map->printers[i].sem) == -1){
            printf("ERROR: sem_destroy()\n");
            return EXIT_FAILURE;
        }

    if(munmap(memory_map, sizeof(memory_map)) == -1){
        printf("ERROR: nummap()\n");
        return EXIT_FAILURE;
    }
    if(shm_unlink(MEMORY_NAME)==-1){
        printf("ERROR: shm_unlink()\n");
        return EXIT_FAILURE;
    };
    return EXIT_SUCCESS;
}