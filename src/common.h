#ifndef __CGOL_COMMON_H__
#define __CGOL_COMMON_H__

#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct X11Env X11Env;

typedef struct XBlockArray
{
    uint32_t** grid;
    uint32_t** next;
    uint32_t   rows;
    uint32_t   cols;

} CGOLMatrix;

typedef struct 
{
    uint32_t width;
    uint32_t height;
    uint8_t  maxsize;
    uint8_t  blackmode;
} X11Display;

typedef struct 
{
    size_t     seed;
    uint32_t   rate;
    X11Display display;
    uint32_t   density;
} CGOLArgs;


#define INFINITE_FACTOR   150
#define INFINITE_FACTOR_2 (INFINITE_FACTOR / 2)

#endif /* __CGOL_COMMON_H__ */
