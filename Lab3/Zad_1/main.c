#include <stdio.h>
#include <time.h>

extern void reverse_by_bite(char *, char *);
extern void reverse_by_block(char *, char *);

int main(int argc, char *argv[]) {
    FILE *pomiar = fopen("pomiar_zad_2.txt", "w");
    double time_start_bite, time_dif_bite, time_start_block, time_dif_block;
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    char *input_file = argv[1];
    char *output_by_bite = "output_by_bite.txt";
    char *output_by_block = "output_by_block.txt";

    time_start_bite =(double)clock()/ CLOCKS_PER_SEC;
    reverse_by_bite(input_file, output_by_bite);
    time_dif_bite = (((double)clock())/CLOCKS_PER_SEC) - time_start_bite;

    time_start_block =(double)clock()/ CLOCKS_PER_SEC;
    reverse_by_block(input_file, output_by_block);
    time_dif_block = (((double)clock())/CLOCKS_PER_SEC) - time_start_block;

    fprintf(pomiar, "Time of copying by bites: %fs\nTime of copying by blocks: %fs\n", time_dif_bite, time_dif_block);
    fclose(pomiar);
    return 0;
}
