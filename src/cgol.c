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

    args->cgol = cgol;

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


void CGOL_newseed(CGOLMatrix* cgol, CGOLArgs* args)
{
    CGOL_clear_grid(cgol);

    if (args->density == 0)
        args->density = 11;

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


uint8_t alive_neighbours(CGOLMatrix* cgol, size_t i, size_t j)
{
    int dx, dy, ni, nj;

    uint8_t neighbours = 0;

    for (dx = -1; dx <= 1; dx++) 
    {
        for (dy = -1; dy <= 1; dy++) 
        {
            if (dx == 0 && dy == 0) continue;

            ni = i + dx;
            nj = j + dy;

            if (ni >= 0 && ni < cgol->rows && nj >= 0 && nj < cgol->cols) 
                neighbours += cgol->grid[ni][nj];
        }
    }

    return neighbours;
}


void CGOL_algorithm(CGOLMatrix* cgol)
{
    uint8_t neighbours;

    for (int i = 0; i < cgol->rows; i++) 
    {
        memset(cgol->next[i], 0, cgol->cols * sizeof(uint32_t));
    }

    for (size_t i = 0; i < cgol->rows; i++) 
    {
        for (size_t j = 0; j < cgol->cols; j++) 
        {
            neighbours = alive_neighbours(cgol, i, j);

            if (1 == cgol->grid[i][j]) 
            {
                if (neighbours < 2 || neighbours > 3) 
                    cgol->next[i][j] = 0; // La cella muore
                else 
                    cgol->next[i][j] = 1; // La cella sopravvive
            } 
            else 
            {
                if (3 == neighbours) cgol->next[i][j] = 1; // La cella si riproduce
            }
        }
    }

    uint32_t** temp = cgol->grid;
    cgol->grid = cgol->next;
    cgol->next = temp;
}
