#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#define L_SLOW 8

pthread_mutex_t mutex01 = PTHREAD_MUTEX_INITIALIZER;

char slownik[L_SLOW][10]={"alfa","bravo","charlie","delta","echo","foxtrot","golf","hotel"};
int NR=0;

void* fun_watka(void* parametr) {
    // zajmij mutex 'mutex01'
    pthread_mutex_lock(&mutex01);

    printf("%s ", slownik[NR++]);
    fflush(stdout);
    if (NR >= L_SLOW) NR = 0;

    // zwolnij mutex 'mutex01'
    pthread_mutex_unlock(&mutex01);

    sleep(1);
    return NULL;
}

int main(void) {
    int i;
    pthread_t watki[20];

    // Utworz 20 watkow realizujacych funkcje 'fun_watka'
    for (i = 0; i < 20; i++) {
        if (pthread_create(&watki[i], NULL, fun_watka, NULL) != 0) {
            printf("blad tworzenia watka %d\n", i);
            return 1;
        }
    }

    // Poczekaj na zakonczenie wszystkich watkow
    for (i = 0; i < 20; i++) {
        if (pthread_join(watki[i], NULL) != 0) {
            printf("blad dolaczania watka %d\n", i);
            return 1;
        }
    }

    printf("\nKONIEC DZIALANIA PROGRAMU\n");
    return 0;
}
Wprowadz
