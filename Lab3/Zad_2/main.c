#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {
    DIR *curr_dir = opendir(".");
    long long sum_of_size = 0;
    if (curr_dir) {
        struct dirent *entry;
        while ((entry = readdir(curr_dir)) != NULL) {
            struct stat file_stat;
            if (stat(entry->d_name, &file_stat) == 0) {
                if (!S_ISDIR(file_stat.st_mode)) {
                    printf("%s %ld\n", entry->d_name, file_stat.st_size);
                    sum_of_size += file_stat.st_size;
                }
            }
        }
        printf("Sum of size: %lld\n", sum_of_size);
        closedir(curr_dir);
    }
    return 0;
}
