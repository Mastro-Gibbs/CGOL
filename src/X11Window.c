#include "X11Window.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>


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



X11Env* CGOL_X11_create(X11Display* display)
{
    XSizeHints* size_hints;

    x11env = malloc(sizeof(X11Env));

    if (NULL == x11env) return NULL;

    x11env->d = XOpenDisplay(NULL);

    if (NULL == x11env->d)
    {
        free(x11env);
        return NULL;
    }

    x11env->s = DefaultScreen(x11env->d);

    display->width  = display->maxsize ? XDisplayWidth(x11env->d, x11env->s)  : display->width,
    display->height = display->maxsize ? XDisplayHeight(x11env->d, x11env->s) : display->height,
    x11env->width      = display->width;
    x11env->height     = display->height;

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

    size_hints = XAllocSizeHints();
    size_hints->flags = PMinSize | PMaxSize;
    size_hints->min_width  = size_hints->max_width  = x11env->width;
    size_hints->min_height = size_hints->max_height = x11env->height;

    XSetWMNormalHints(x11env->d, x11env->w, size_hints);
    XFree(size_hints);

    XSelectInput(x11env->d, x11env->w, KeyPressMask | ExposureMask | ButtonPressMask);

    x11env->gc = XCreateGC(x11env->d, x11env->w, 0, NULL);
    
    XMapWindow(x11env->d, x11env->w);

    XStoreName(x11env->d, x11env->w, "CGOL: Conway's Game Of Life");

    x11env->colormap = DefaultColormap(x11env->d, x11env->s);

    XParseColor(x11env->d, x11env->colormap, "#EDEEEF", &x11env->grid_color);
    XAllocColor(x11env->d, x11env->colormap, &x11env->grid_color);

    XParseColor(x11env->d, x11env->colormap, "#333333", &x11env->cell_color);
    XAllocColor(x11env->d, x11env->colormap, &x11env->cell_color);

    XSetForeground(x11env->d, x11env->gc, x11env->cell_color.pixel);

    return x11env;
}


void CGOL_X11_clear(void)
{
    XClearWindow(x11env->d, x11env->w);

    XSetForeground(x11env->d, x11env->gc, x11env->grid_color.pixel);

    for (uint32_t i = 0; i < x11env->width; i+=10)
    {
        XDrawLine(x11env->d, x11env->w, x11env->gc, 0, i, x11env->width, i);
        XDrawLine(x11env->d, x11env->w, x11env->gc, i, 0, i, x11env->height);
    }

    XSetForeground(x11env->d, x11env->gc, x11env->cell_color.pixel);

    XFlush(x11env->d);
}


void CGOL_X11_next_evt(CGOLArgs* args)
{
    XEvent xevt;
    uint32_t col, row, val;

    if (XPending(x11env->d))
    {
        XNextEvent(x11env->d, &xevt);
        
        switch (xevt.type)
        {

        case ButtonPress:
            if (args->xargs.suspend == 1)
            {
                row = (INFINITE_FACTOR_2 - 1) + (xevt.xbutton.y / 10);
                col = (INFINITE_FACTOR_2 - 1) + (xevt.xbutton.x / 10);
                val = args->cgol->grid[row][col];

                args->cgol->grid[row][col] = !val;

                CGOL_X11_draw_grid(args->cgol);
            }
            break;

        case KeyPress:
            if (xevt.xkey.keycode == 39) args->xargs.suspend = !args->xargs.suspend; // s
            if (xevt.xkey.keycode == 24) args->xargs.exit    = 1; // q
            if (xevt.xkey.keycode == 57) args->xargs.newseed = 1; // n
            
            if (args->xargs.suspend)
            {
                if (xevt.xkey.keycode == 54) args->xargs.clear   = 1; // c
            }

            if (xevt.xkey.keycode == 52) // z
            {
                if (args->rate > 1) args->rate -= 1;
            }

            if (xevt.xkey.keycode == 53) // x
            {
                if (args->rate < 50) args->rate += 1;
            }

            break;

        case Expose:
            args->xargs.exposed = 1;
            break;
        
        default:
            break;
        }

        args->xargs.evt = 1;
    }
    else args->xargs.evt = 0;
}

void CGOL_X11_draw_grid(const CGOLMatrix* cgol)
{
    if (NULL != x11env)
    {
        for (size_t i = INFINITE_FACTOR_2 - 1, xi = 0; i < cgol->rows - INFINITE_FACTOR_2; ++i, xi+=10)
        {
            for (size_t j = INFINITE_FACTOR_2 - 1, xj = 0; j < cgol->cols - INFINITE_FACTOR_2; ++j, xj+=10)
            {
                if (cgol->grid[i][j] == 1)
                {
                    XFillRectangle(
                        x11env->d, 
                        x11env->w, 
                        x11env->gc,
                        xj,
                        xi,
                        9,
                        9
                    );
                }
            }
        }

        XFlush(x11env->d);
    }
}

void CGOL_X11_delete_env(void)
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
