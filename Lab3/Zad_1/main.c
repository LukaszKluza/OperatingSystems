extern void reverse_by_bite(char *, char *);
extern void reverse_by_block(char *, char *);

int main(){
    char *input = "input.txt";
    char *output_by_bite = "output_by_bite.txt";
    char *output_by_block = "output_by_block.txt";

    reverse_by_bite(input, output_by_bite);
    reverse_by_block(input, output_by_block);

    return 0;
}
