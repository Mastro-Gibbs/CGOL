#include "X11Window.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>

/**
 * implementation for X11Env struct
*/
struct X11Env
{
    Display* d;
    Window   w;

    GC       gc;
    int      s;

    Colormap colormap;
    XColor   grid_color;
    XColor   cell_color;

    uint32_t width;
    uint32_t height;
};


X11Env* x11env = NULL;


/**
 * @brief  set X windows fixed size
 * @return void
 * @param  void
*/
void X11_fixed_size(void)
{
    XSizeHints* size_hints; // size hints to 

    size_hints = XAllocSizeHints();
    size_hints->flags = PMinSize | PMaxSize;
    size_hints->min_width  = size_hints->max_width  = x11env->width;
    size_hints->min_height = size_hints->max_height = x11env->height;

    XSetWMNormalHints(x11env->d, x11env->w, size_hints);
    XFree(size_hints);
}


X11Env* X11_create(X11Display* display)
{
    x11env = malloc(sizeof(X11Env)); // allocate env

    if (NULL == x11env) return NULL;

    x11env->d = XOpenDisplay(NULL);  // open display

    if (NULL == x11env->d)
    {
        free(x11env);
        return NULL;
    }

    x11env->s = DefaultScreen(x11env->d); // select default screen

    // chose window width (max, prefered)
    display->width  = display->maxsize ? XDisplayWidth(x11env->d, x11env->s)  : display->width;

    // chose window height (max, prefered)
    display->height = display->maxsize ? XDisplayHeight(x11env->d, x11env->s) : display->height;

    // store this infos
    x11env->width   = display->width;
    x11env->height  = display->height;

    // create X window
    x11env->w = XCreateSimpleWindow(
        x11env->d,
        RootWindow(x11env->d, x11env->s),
        0,
        0, 
        x11env->width,
        x11env->height,
        1,
        BlackPixel(x11env->d, x11env->s),
        WhitePixel(x11env->d, x11env->s)
    );

    // set window fixed size
    X11_fixed_size(); 

    // chose event listers
    XSelectInput(x11env->d, x11env->w, KeyPressMask | ExposureMask | ButtonPressMask);

    // create a GC
    x11env->gc = XCreateGC(x11env->d, x11env->w, 0, NULL);
    
    // set window name
    XStoreName(x11env->d, x11env->w, "CGOL: Conway's Game Of Life");

    // get default colormap
    x11env->colormap = DefaultColormap(x11env->d, x11env->s);

    // parse color for grid
    XParseColor(x11env->d, x11env->colormap, GRID_COLOR, &x11env->grid_color);
    XAllocColor(x11env->d, x11env->colormap, &x11env->grid_color);

    // parse color for cells
    XParseColor(x11env->d, x11env->colormap, CELL_COLOR, &x11env->cell_color);
    XAllocColor(x11env->d, x11env->colormap, &x11env->cell_color);

    // set color for cells
    XSetForeground(x11env->d, x11env->gc, x11env->cell_color.pixel);

    XMapWindow(x11env->d, x11env->w);

    return x11env;
}


void X11_clear(CGOLArgs* args)
{
    // clear content
    XClearWindow(x11env->d, x11env->w); 

    // set color for grid
    XSetForeground(x11env->d, x11env->gc, x11env->grid_color.pixel);

    // draw grid
    for (uint32_t i = 0; i < x11env->width; i+=args->gridsize)
    {
        XDrawLine(x11env->d, x11env->w, x11env->gc, 0, i, x11env->width, i);
        XDrawLine(x11env->d, x11env->w, x11env->gc, i, 0, i, x11env->height);
    }

    // restore color for cells
    XSetForeground(x11env->d, x11env->gc, x11env->cell_color.pixel);

    // flush the display
    XFlush(x11env->d);
}


void X11_next_evt(CGOLArgs* args)
{
    XEvent xevt;
    uint32_t col, row, val;

    // if there is an event in queue
    if (XPending(x11env->d))
    {
        // get it
        XNextEvent(x11env->d, &xevt);
        
        switch (xevt.type)
        {
        
        // mouse press event
        case ButtonPress:
            // is execution in suspend
            // compute mouse click point
            // toggle corresponding cell
            if (args->XEvtFlags & XEVENT_SUSPEND)
            {
                row = (INFINITE_FACTOR_2 - 1) + (xevt.xbutton.y / args->gridsize);
                col = (INFINITE_FACTOR_2 - 1) + (xevt.xbutton.x / args->gridsize);
                val = args->cgol->grid[row][col];

                args->cgol->grid[row][col] = !val;

                // draw updated grid
                X11_draw(args);
            }
            break;

        // keyboard press event
        case KeyPress:
            switch (xevt.xkey.keycode)
            {
            // start/stop game
            case KEY_S:  
                XEVENT_CHG(args->XEvtFlags, XEVENT_SUSPEND);
                break;

            // quit game
            case KEY_Q:
                XEVENT_SET(args->XEvtFlags, XEVENT_EXIT);
                break;

            // new game
            case KEY_N: 
                XEVENT_SET(args->XEvtFlags, XEVENT_NEWGAME);
                break;

            // clear game
            case KEY_C:
                if (args->XEvtFlags & XEVENT_SUSPEND)
                    XEVENT_SET(args->XEvtFlags, XEVENT_CLEAR);
                break;

            // decrease cycle rate 
            case KEY_Z:
                if (args->rate > DEFAULT_MIN_RATE) 
                    args->rate -= 1;
                break;

            // increase cycle rate
            case KEY_X:
                if (args->rate < DEFAULT_MAX_RATE) 
                {
                    if (args->gridsize < DEFAULT_MIN_GRID_SIZE && args->rate >= DEFAULT_MAX_RATE_2)
                        args->rate += DEFAULT_MAX_RATE_2;
                    else
                        args->rate += 1;
                }
                break;

            default:
                break;
            }

            break;

        // X window showing
        case Expose:
            XEVENT_SET(args->XEvtFlags, XEVENT_EXPOSED);
            break;
        
        default:
            break;
        }

        // an event appear
        XEVENT_SET(args->XEvtFlags, XEVENT_EVT);
    }
    // no event in queue
    else XEVENT_CLR(args->XEvtFlags, XEVENT_EVT);
}

void X11_draw(CGOLArgs* args)
{
    CGOLMatrix* cgol = args->cgol;

    X11_clear(args);

    // using INFINITE_FACTOR_2 to get visible area
    for (size_t i = INFINITE_FACTOR_2 - 1, xi = 0; i < cgol->rows - INFINITE_FACTOR_2; ++i, xi+=args->gridsize)
    {
        for (size_t j = INFINITE_FACTOR_2 - 1, xj = 0; j < cgol->cols - INFINITE_FACTOR_2; ++j, xj+=args->gridsize)
        {
            // if cell is alive draw it
            if (cgol->grid[i][j] == 1)
            {
                XFillRectangle(
                    x11env->d, 
                    x11env->w, 
                    x11env->gc,
                    xj,
                    xi,
                    args->cellsize,
                    args->cellsize
                );
            }
        }
    }

    XFlush(x11env->d);
}

void X11_release(void)
{
    if (NULL != x11env)
    {
        XFreeGC(x11env->d, x11env->gc);
        XDestroyWindow(x11env->d, x11env->w);
        XCloseDisplay(x11env->d);

        free(x11env);

        x11env = NULL;
    }
}
