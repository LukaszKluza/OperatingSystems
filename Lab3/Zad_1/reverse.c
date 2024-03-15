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

void reverse_by_block(char* input_file, char* output_file){
    FILE *input =  fopen ( input_file , "r");
    FILE *output =  fopen ( output_file , "w");
    char buffer[BLOCK_SIZE];
    int idx = -BLOCK_SIZE;

    if(input && output){
        while(fseek(input, idx ,SEEK_END) == 0){
            fread(&buffer, sizeof(char),BLOCK_SIZE,input);
            fwrite(&buffer, sizeof(char),BLOCK_SIZE,output);
            idx -= BLOCK_SIZE;
        }
        fclose(input);
        fclose(output);
    }
}