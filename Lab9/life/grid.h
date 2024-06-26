#pragma once
#include <stdbool.h>

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
void *update_cells(void *args);
int init_threads(char *src, char *dst, int threads_num);
int update_grid_multi_thread();
