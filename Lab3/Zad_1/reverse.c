#include <stdio.h>
#include "reverse.h"

void reverse_by_bite(char* input_file, char* output_file){
    FILE *input =  fopen ( input_file , "r");
    FILE *output =  fopen ( output_file , "w");
    char buffer;
    int idx = 0;

    if(input && output){
        while(fseek(input, --idx ,SEEK_END) == 0){
            fread(&buffer, sizeof(char),1,input);
            fwrite(&buffer, sizeof(char),1,output);
        }
        fclose(input);
        fclose(output);
    }
}

void reverse_by_block(char *input_file, char *output_file) {
    FILE *input = fopen(input_file, "r");
    FILE *output = fopen(output_file, "w");
    int block_size = 1024;
    char buffer[block_size];
    ssize_t res, file_size;
    int idx = -1;

    if(input && output){
        fseek(input, 0, SEEK_END);
        file_size = ftell(input);
        while (fseek(input, idx * block_size, SEEK_END) == 0) {
            res = fread(buffer, sizeof(char), block_size, input);
            for (int i = 0; i < res / 2; i++) {
                char temp = buffer[i];
                buffer[i] = buffer[res - i - 1];
                buffer[res - i - 1] = temp;
            }
            fwrite(buffer, sizeof(char), res, output);
            idx--;
        }
        fseek(input, 0, SEEK_SET);
        res = fread(buffer, sizeof(char), file_size + (idx +1)*block_size, input);
        for (int i = 0; i < res / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[res - i - 1];
            buffer[res - i - 1] = temp;
        }
        fwrite(buffer, sizeof(char), res, output);
    }

    fclose(input);
    fclose(output);
}
