#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

static int work = 1;

void handler(){
    work = 0;
}


int main(int argc, char* argv[])
{
    if(argc != 2){
        printf("ERROR: Too small arguments!\n");
        return -1;
    }
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);

    size_t n = atoi(argv[1]);
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);
    if(init_threads(foreground, background,n) == -1){
        printf("ERROR: Wrong numbers of threads!\n");
    };

	while (work)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
        if(update_grid_multi_thread()==-1){
            printf("ERROR: Init threads before!\n");
        }
		tmp = foreground;
		foreground = background;
		background = tmp;
	}
    endwin(); // End curses mode
    destroy_grid(foreground);
    destroy_grid(background);
}
