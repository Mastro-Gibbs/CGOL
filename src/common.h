#ifndef __CGOL_COMMON_H__
#define __CGOL_COMMON_H__

#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

// hold time in microseconds
typedef unsigned long long ttime_t;

// uncomplete struct for X11 environment
typedef struct X11Env X11Env;

// hold CGOL data
typedef struct CGOLMatrix
{
    uint32_t** grid; // curr matrix 
    uint32_t** next; // next state
    uint32_t   rows; // rows count
    uint32_t   cols; // cols count

} CGOLMatrix;


// hold X11 display settings
typedef struct 
{
    uint32_t width;
    uint32_t height;
    uint8_t  maxsize; 
    
} X11Display;


// hold CGOL args
typedef struct 
{
    size_t   seed;
    uint8_t  rate;
    uint8_t  density;
    uint8_t  gridsize;
    uint8_t  cellsize;

    ssize_t  btime;
    
    // X event flags
    // exposed | evt | suspend | clear | newgame | exit 
    volatile uint8_t XEvtFlags;

    X11Display  display;
    CGOLMatrix* cgol;
    
} CGOLArgs;


// X windows default sizes
#define X11WND_DEFAULT_WIDTH  640u
#define X11WND_DEFAULT_HEIGHT 480u


// X event flag ops 
#define XEVENT_SET(bf, b)     (bf |=  b)
#define XEVENT_CLR(bf, b)     (bf &= ~b)
#define XEVENT_CHG(bf, b)     (bf ^=  b)
 
 
// X event flags getters 
#define XEVENT_EXIT           (1 << 0)
#define XEVENT_NEWGAME        (1 << 1)
#define XEVENT_CLEAR          (1 << 2)
#define XEVENT_SUSPEND        (1 << 3)
#define XEVENT_EVT            (1 << 4)
#define XEVENT_EXPOSED        (1 << 5)
 
 
// X event facilities getters 
#define CGOL_EDITOR_MODE      (XEVENT_SUSPEND | XEVENT_CLEAR)
#define CGOL_CLEAR            (XEVENT_EXPOSED | XEVENT_SUSPEND | XEVENT_CLEAR)
#define CGOL_NEWGAME          (XEVENT_EXPOSED | XEVENT_NEWGAME)
#define CGOL_FORCE_REDRAW     (XEVENT_EXPOSED | XEVENT_EVT)
#define CGOL_CYCLE(flags)     ((flags & XEVENT_EXPOSED) && !(flags & XEVENT_EVT) && !(flags & XEVENT_SUSPEND))
#define CGOL_DO(flags)        (0 == (flags & XEVENT_EXIT))
 
 
// matrix off-screen size 
// it symulates an infinite matrix 
#define INFINITE_FACTOR       150u                   
#define INFINITE_FACTOR_2     (INFINITE_FACTOR / 2)
 
 
// keyboard keycodes 
#define KEY_Z                 52u
#define KEY_X                 53u
#define KEY_Q                 24u
#define KEY_S                 39u
#define KEY_C                 54u
#define KEY_N                 57u
 
 
// default arg values 
#define DEFAULT_RATE          4u
#define DEFAULT_DENSITY       11u
#define DEFAULT_MIN_RATE      1u
#define DEFAULT_MAX_RATE      40u
#define DEFAULT_MAX_RATE_2    (DEFAULT_MAX_RATE / 2)
#define DEFAULT_MIN_DENSITY   5u
#define DEFAULT_MAX_DENSITY   50u
#define DEFAULT_XEVENT_FLAGS  ((uint8_t) (XEVENT_EVT))
 
 
// sleep value 
#define MAX_SLEEP_TIME        999999u
 
 
// grid data 
#define GRID_COLOR            "#EDEEEF"
#define DEFAULT_GRID_SIZE     10u
#define DEFAULT_MIN_GRID_SIZE 2u
#define DEFAULT_MAX_GRID_SIZE 50u


// cell data 
#define CELL_COLOR            "#333333"
#define DEFAULT_CELL_SIZE     (DEFAULT_GRID_SIZE - 1u)
 
 
// icon
#define USE_ICON_LARGE        1
 
 
#endif /* __CGOL_COMMON_H__ */
