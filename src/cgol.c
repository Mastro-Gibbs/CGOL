#include "cgol.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>


uint8_t CGOL_hash(size_t a, size_t b)
{
    a = (a ^ 0xdeadbeef) + (a ^ 0xbeefdead);
    a ^= (b >> 17);
    a *= 0xc0ffeeee;
    a ^= (b << 7);
    a *= 0x7afb6c3d;
    a ^= (b >> 4);
    return (a % 100) < 11;
}


CGOLMatrix* CGOL_init_grid(const CGOLArgs* args)
{
    CGOLMatrix* grid = malloc(sizeof(CGOLMatrix));

    if (NULL == grid) return NULL;

    grid->cols = (args->display.width  / 10) + INFINITE_FACTOR;
    grid->rows = (args->display.height / 10) + INFINITE_FACTOR;

    uint32_t** _grid = malloc(sizeof(uint32_t*) * grid->rows);
    uint32_t** _next = malloc(sizeof(uint32_t*) * grid->rows);

    if (NULL == _grid || NULL == _next) 
    {        
        if (NULL != _grid) free(_grid);
        if (NULL != _next) free(_next);

        return NULL;
    }

    for(int i = 0; i < grid->rows; i++) 
    {
        _grid[i] = calloc(grid->cols, sizeof(uint32_t));
        _next[i] = calloc(grid->cols, sizeof(uint32_t));

        if (NULL == _grid[i] || NULL == _next[i]) 
        { 
            for (int j = 0; j < i; j++) 
            {
                if (NULL != _grid[i]) free(_grid[j]);
                if (NULL != _next[i]) free(_next[j]);
            }

            free(_grid);
            free(_next);

            return NULL;
        }

        srand(i * 42);

        for (int j = 0; j < grid->cols; j++) 
        {
            _grid[i][j] = CGOL_hash(args->seed, j * rand());
        }
    }

    grid->grid = _grid;
    grid->next = _next;

    return grid;
}


void CGOL_release_grid(CGOLMatrix* grid)
{
    if (NULL != grid)
    {
        for(int i = 0; i < grid->rows; i++)
        {
            if (grid->grid[i]) free(grid->grid[i]);
            if (grid->next[i]) free(grid->next[i]);
        }

        if (grid->grid) free(grid->grid);
        if (grid->next) free(grid->next);

        free(grid);
    }
}


void CGOL_algorithm(CGOLMatrix* grid)
{
    for (int i = 0; i < grid->rows; i++) 
    {
        memset(grid->next[i], 0, grid->cols *  sizeof(uint32_t));
    }

    for (int i = 0; i < grid->rows; i++) 
    {
        for (int j = 0; j < grid->cols; j++) 
        {
            int aliveNeighbours = 0;

            for (int dx = -1; dx <= 1; dx++) 
            {
                for (int dy = -1; dy <= 1; dy++) 
                {
                    if (dx == 0 && dy == 0) continue;

                    int ni = i + dx;
                    int nj = j + dy;

                    if (ni >= 0 && ni < grid->rows && nj >= 0 && nj < grid->cols) 
                    {
                        aliveNeighbours += grid->grid[ni][nj];
                    }
                }
            }

            if (1 == grid->grid[i][j]) 
            {
                // La cella è viva
                if (aliveNeighbours < 2 || aliveNeighbours > 3) 
                {
                    grid->next[i][j] = 0; // La cella muore
                } 
                else 
                {
                    grid->next[i][j] = 1; // La cella sopravvive
                }
            } 
            else 
            {
                // La cella è morta
                if (3 == aliveNeighbours) 
                {
                    grid->next[i][j] = 1; // La cella si riproduce
                }
            }
        }
    }

    uint32_t** temp = grid->grid;
    grid->grid = grid->next;
    grid->next = temp;
}
