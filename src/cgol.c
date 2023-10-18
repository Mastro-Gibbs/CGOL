#include "cgol.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>


uint8_t CGOL_hash(size_t a, size_t b, uint32_t d)
{
    a -= (a ^ 0xdeadbeef) + (b ^ 0xbeefdead);
    a ^= (a >> 17);
    a *= 0xc0ffeeee;
    a ^= (a << 7);
    a *= 0x7afb6c3d;
    a ^= (a >> 4);
    return (a % 100) < d;
}


CGOLMatrix* CGOL_init_grid(CGOLArgs* args)
{
    CGOLMatrix* cgol = malloc(sizeof(CGOLMatrix));

    if (NULL == cgol) return NULL;

    args->xargs.grid = cgol;

    cgol->cols = (args->display.width  / 10) + INFINITE_FACTOR;
    cgol->rows = (args->display.height / 10) + INFINITE_FACTOR;

    uint32_t** _grid = malloc(sizeof(uint32_t*) * cgol->rows);
    uint32_t** _next = malloc(sizeof(uint32_t*) * cgol->rows);

    if (NULL == _grid || NULL == _next) 
    {        
        if (NULL != _grid) free(_grid);
        if (NULL != _next) free(_next);

        return NULL;
    }

    for(int i = 0; i < cgol->rows; i++) 
    {
        _grid[i] = calloc(cgol->cols, sizeof(uint32_t));
        _next[i] = calloc(cgol->cols, sizeof(uint32_t));

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

        for (int j = 0; j < cgol->cols; j++) 
        {
            _grid[i][j] = CGOL_hash(args->seed, j * rand(), args->density);
        }
    }

    cgol->grid = _grid;
    cgol->next = _next;

    return cgol;
}



void CGOL_clear_grid(CGOLMatrix* cgol)
{
    for (int i = 0; i < cgol->rows; i++) 
    {
        memset(cgol->grid[i], 0, cgol->cols * sizeof(uint32_t));
        memset(cgol->next[i], 0, cgol->cols * sizeof(uint32_t));
    }
}


void CGOL_newseed(CGOLMatrix* cgol, const CGOLArgs* args)
{
    CGOL_clear_grid(cgol);

    for (int i = 0; i < cgol->rows; i++) 
    {
        for (int j = 0; j < cgol->cols; j++) 
        {
            cgol->grid[i][j] = CGOL_hash(args->seed, j * rand(), args->density);
        }
    }
}


void CGOL_release_grid(CGOLMatrix* cgol)
{
    if (NULL != cgol)
    {
        for(int i = 0; i < cgol->rows; i++)
        {
            if (cgol->grid[i]) free(cgol->grid[i]);
            if (cgol->next[i]) free(cgol->next[i]);
        }

        if (cgol->grid) free(cgol->grid);
        if (cgol->next) free(cgol->next);

        free(cgol);
    }
}


void CGOL_algorithm(CGOLMatrix* cgol)
{
    for (int i = 0; i < cgol->rows; i++) 
    {
        memset(cgol->next[i], 0, cgol->cols * sizeof(uint32_t));
    }

    for (int i = 0; i < cgol->rows; i++) 
    {
        for (int j = 0; j < cgol->cols; j++) 
        {
            int aliveNeighbours = 0;

            for (int dx = -1; dx <= 1; dx++) 
            {
                for (int dy = -1; dy <= 1; dy++) 
                {
                    if (dx == 0 && dy == 0) continue;

                    int ni = i + dx;
                    int nj = j + dy;

                    if (ni >= 0 && ni < cgol->rows && nj >= 0 && nj < cgol->cols) 
                    {
                        aliveNeighbours += cgol->grid[ni][nj];
                    }
                }
            }

            if (1 == cgol->grid[i][j]) 
            {
                // La cella è viva
                if (aliveNeighbours < 2 || aliveNeighbours > 3) 
                {
                    cgol->next[i][j] = 0; // La cella muore
                } 
                else 
                {
                    cgol->next[i][j] = 1; // La cella sopravvive
                }
            } 
            else 
            {
                // La cella è morta
                if (3 == aliveNeighbours) 
                {
                    cgol->next[i][j] = 1; // La cella si riproduce
                }
            }
        }
    }

    uint32_t** temp = cgol->grid;
    cgol->grid = cgol->next;
    cgol->next = temp;
}
