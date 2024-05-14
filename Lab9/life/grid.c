#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>

const int grid_width = 30;
const int grid_height = 30;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
}

//multi-threading

struct ThreadArgs{
    char *src;
    char *dst;
    size_t start;
    size_t cells_num;
};

pthread_t *threads = NULL;
size_t threads_num_ = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int init_threads(char *src, char *dst, int threads_num){
    if(threads == NULL){
        if(threads_num <=0 || threads_num > grid_width*grid_height){
            return -1;
        }
        threads_num_ = threads_num;
        threads = malloc(threads_num*sizeof(pthread_t));
        size_t cells_num = grid_width*grid_height/threads_num;
        size_t mod = grid_width*grid_height - cells_num;
        size_t curr_beginning = 0;
        for(size_t i = 0; i< threads_num_; ++i){
            struct ThreadArgs *args = malloc(sizeof(struct ThreadArgs));
            args->src = src;
            args->dst = dst;
            args->start = curr_beginning;
            args->cells_num = cells_num + (mod > 0 ? 1 : 0);
            curr_beginning += args->cells_num;
            mod--;
            pthread_create(&threads[i], NULL, update_cells, args);
        }
    }
    return 0;
}

void *update_cells(void *args){
    struct ThreadArgs *thread_args = (struct ThreadArgs*)args;

    pthread_mutex_lock(&mutex);
    while(true) {
        pthread_cond_wait(&cond, &mutex);
        for (size_t i = thread_args->start; i < thread_args->start + thread_args->cells_num; ++i) {
            thread_args->dst[i] = is_alive(i / grid_width, i % grid_width, thread_args->src);
            char *temp = thread_args->src;
            thread_args->src = thread_args->dst;
            thread_args->dst = temp;
        }
    }
    pthread_mutex_unlock(&mutex);
    free(thread_args);
    return NULL;
}


int update_grid_multi_thread(){
    if(threads == NULL){
        return -1;
    }
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    return 0;
}
