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

    uint32_t width;
    uint32_t height;
};


X11Env* m_env = NULL;



X11Env* CGOL_X11_create(X11Display* display,
                        const uint8_t set)
{
    XSizeHints* size_hints;

    X11Env* env = malloc(sizeof(X11Env));

    if (NULL == env) return NULL;


    env->d = XOpenDisplay(NULL);

    if (NULL == env->d)
    {
        free(env);
        return NULL;
    }

    env->s = DefaultScreen(env->d);

    display->width  = display->maxsize ? XDisplayWidth(env->d, env->s)  : display->width,
    display->height = display->maxsize ? XDisplayHeight(env->d, env->s) : display->height,
    env->width      = display->width;
    env->height     = display->height;

    env->w = XCreateSimpleWindow(
        env->d,
        RootWindow(env->d, env->s),
        0, 
        0,
        env->width,
        env->height,
        1,
        BlackPixel(env->d, env->s),
        WhitePixel(env->d, env->s)
    );

    size_hints = XAllocSizeHints();
    size_hints->flags = PMinSize | PMaxSize;
    size_hints->min_width  = size_hints->max_width  = env->width;
    size_hints->min_height = size_hints->max_height = env->height;

    XSetWMNormalHints(env->d, env->w, size_hints);
    XFree(size_hints);

    XSelectInput(env->d, env->w, KeyPressMask | ExposureMask | ButtonPressMask);

    env->gc = XCreateGC(env->d, env->w, 0, NULL);
    
    XMapWindow(env->d, env->w);

    XSetForeground(env->d, env->gc, BlackPixel(env->d, env->s));

    XStoreName(env->d, env->w, "CGOL: Conway's Game Of Life");

    env->colormap = DefaultColormap(env->d, env->s);

    XParseColor(env->d, env->colormap, "#EDEEEF", &env->grid_color);
    XAllocColor(env->d, env->colormap, &env->grid_color);

    m_env = env;

    return env;
}


void CGOL_X11_clear(void)
{
    XClearWindow(m_env->d, m_env->w);

    XSetForeground(m_env->d, m_env->gc, m_env->grid_color.pixel);

    for (uint32_t i = 0; i < m_env->width; i+=10)
    {
        XDrawLine(m_env->d, m_env->w, m_env->gc, 0, i, m_env->width, i);
        XDrawLine(m_env->d, m_env->w, m_env->gc, i, 0, i, m_env->height);
    }

    XSetForeground(m_env->d, m_env->gc, BlackPixel(m_env->d, m_env->s));

    XFlush(m_env->d);
}


void CGOL_X11_next_evt(CGOLArgs* args)
{
    XEvent xevt;
    uint32_t col, row, val;

    if (XPending(m_env->d))
    {
        XNextEvent(m_env->d, &xevt);
        
        switch (xevt.type)
        {

        case ButtonPress:
            if (args->xargs.suspend == 1)
            {
                row = (INFINITE_FACTOR_2 - 1) + (xevt.xbutton.y / 10);
                col = (INFINITE_FACTOR_2 - 1) + (xevt.xbutton.x / 10);
                val = args->xargs.grid->grid[row][col];

                args->xargs.grid->grid[row][col] = !val;

                CGOL_X11_draw_grid(args->xargs.grid);
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
    if (NULL != m_env)
    {
        for (size_t i = INFINITE_FACTOR_2 - 1, xi = 0; i < cgol->rows - INFINITE_FACTOR_2; ++i, xi+=10)
        {
            for (size_t j = INFINITE_FACTOR_2 - 1, xj = 0; j < cgol->cols - INFINITE_FACTOR_2; ++j, xj+=10)
            {
                if (cgol->grid[i][j] == 1)
                {
                    XFillRectangle(
                        m_env->d, 
                        m_env->w, 
                        m_env->gc,
                        xj,
                        xi,
                        9,
                        9
                    );
                }
            }
        }

        XFlush(m_env->d);
    }
}

void CGOL_X11_delete_env(void)
{
    if (NULL != m_env)
    {
        XFreeGC(m_env->d, m_env->gc);
        XDestroyWindow(m_env->d, m_env->w);
        XCloseDisplay(m_env->d);

        free(m_env);

        m_env = NULL;
    }
}
