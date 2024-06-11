#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

# define NUM_REIN 9

size_t readyReindeers = 0;
size_t toDeliver = 4;
size_t delivered = 0;


pthread_mutex_t reindeersMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeerCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t holidaysMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t holidaysCond = PTHREAD_COND_INITIALIZER;

pthread_t reindeers[9];
pthread_t santa;

struct ThreadArgs{
    size_t id;
};


void* holidays(void *args){
    struct ThreadArgs *thread_args = (struct ThreadArgs*)args;
    size_t id = thread_args->id;
    srand(time(NULL) + id);

    while(1){
        size_t timeOfSleep = rand()% 6 + 5;
        sleep(timeOfSleep);

        pthread_mutex_lock(&reindeersMutex);
        readyReindeers += 1;
        printf("Komunikat: Renifer: czeka %zu reniferów na Mikołaja, %zu\n", readyReindeers, id );
        if(readyReindeers == NUM_REIN){
            pthread_cond_signal(&reindeerCond);
        }
        pthread_mutex_unlock(&reindeersMutex);

        pthread_mutex_lock(&holidaysMutex);
        pthread_cond_wait(&holidaysCond, &holidaysMutex);
        pthread_mutex_unlock(&holidaysMutex);

        printf("Komunikat: Renifer %zu leci na wakacje\n", id);
    }
    free(thread_args);
    return NULL;
}


void deliverGifts(){
    size_t timeOfSleep = rand()% 3 + 2;
    printf("Mikołaj dostarcza prezenty...\n");
    sleep(timeOfSleep);
    delivered++;
    printf("Mikołaj skończył dostarczać prezenty.\n");
}
void* santaClaus(){
    while(delivered < toDeliver){
        pthread_mutex_lock(&reindeersMutex);
        while (readyReindeers < NUM_REIN) {
            pthread_cond_wait(&reindeerCond, &reindeersMutex);
        }
        pthread_mutex_unlock(&reindeersMutex);
        pthread_mutex_lock(&holidaysMutex);
        deliverGifts();
        readyReindeers = 0;
        pthread_cond_broadcast(&holidaysCond);
        pthread_mutex_unlock(&holidaysMutex);

        printf("Mikołaj idzie spać.\n");
    }
    for (int j = 0; j < NUM_REIN; j++) {
        pthread_cancel(reindeers[j]);
    }
    return NULL;
}


int main(int argc, char* args[]){
    if(argc == 2){
        toDeliver = atoi(args[1]);
    }
    srand(time(NULL));

    pthread_create(&santa, NULL, santaClaus, NULL);

    for(size_t i =0; i< NUM_REIN; ++i){
        struct ThreadArgs *arg = malloc(sizeof(struct ThreadArgs));
        arg->id = i + 1;
        pthread_create(&reindeers[i],NULL, holidays, arg);
    }
    for (size_t i = 0; i < NUM_REIN; ++i) {
        pthread_join(reindeers[i], NULL);
    }

    pthread_join(santa, NULL);

    return 0;
}