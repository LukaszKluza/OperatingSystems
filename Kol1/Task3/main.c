#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>


int main(int argc, char* argv[])
{

 if(argc !=4){
    printf("Not a suitable number of program parameters\n");
    return(1);
 }
    pid_t child = fork();
    if(child==0){
        char operation = *argv[2];
        if(operation != '+' && operation != '-' && operation != 'x' && operation != '/'){
            printf("ERROR\n");
            return 3;
        }
        int op2 = atoi(argv[3]);
        if(op2 == 0){
            printf("ERROR\n");
            return 3;
        }
        char* const av[] = {argv[1], argv[2], argv[3]};
        execvp("./calc", av);
    }
    //utworz proces potomny w ktorym wykonasz program ./calc z parametrami argv[1], argv[2] oraz argv[3]
    //odpowiednio jako pierwszy operand, operacja (+-x/) i drugi operand 
    //uzyj tablicowego sposobu przekazywania parametrow do programu
 return 0;
}
