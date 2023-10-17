#include "X11Window.h"

#include <X11/Xlib.h>
#include <stdlib.h>


struct X11Env
{
    Display* d;
    Window   w;

    GC       gc;
    int      s;
};


X11Env* m_env = NULL;


X11Env* CGOL_X11_create(X11Display* display,
                        const uint8_t set)
{
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

    env->w = XCreateSimpleWindow(
        env->d,
        RootWindow(env->d, env->s),
        0, 
        0,
        display->maxsize ? XDisplayWidth(env->d, env->s)  : display->width,
        display->maxsize ? XDisplayHeight(env->d, env->s) : display->height,
        1,
        BlackPixel(env->d, env->s),
        display->blackmode ? BlackPixel(env->d, env->s) : WhitePixel(env->d, env->s)
    );

    env->gc = XCreateGC(env->d, env->w, 0, NULL);
    
    XSelectInput(env->d, env->w, ExposureMask | KeyPressMask);
    XMapWindow(env->d, env->w);

    XSetForeground(env->d, env->gc, display->blackmode ? WhitePixel(env->d, env->s) : BlackPixel(env->d, env->s));

    if (0 != set) m_env = env;

    return env;
}


void CGOL_X11_set_env(X11Env* env)
{
    m_env = env;
}


void CGOL_X11_next_evt(volatile uint32_t* evt, 
                       volatile uint32_t* exit, 
                       volatile uint32_t* exposed)
{
    XEvent xevt;

    if (XPending(m_env->d))
    {
        XNextEvent(m_env->d, &xevt);
        
        switch (xevt.type)
        {
        case KeyPress:
            *exit = 1;
            break;

        case Expose:
            *exposed = 1;
            break;
        
        default:
            break;
        }

        *evt = 1;
    }
    else *evt = 0;
}

void CGOL_X11_draw_grid(const CGOLMatrix* grid)
{
    if (NULL != m_env)
    {
        XClearWindow(m_env->d, m_env->w);
        XFlush(m_env->d);

        for (size_t i = INFINITE_FACTOR_2 - 1; i < grid->rows - INFINITE_FACTOR_2; ++i)
        {
            for (size_t j = INFINITE_FACTOR_2 - 1; j < grid->cols - INFINITE_FACTOR_2; ++j)
            {
                if (grid->grid[i][j] == 1)
                {
                    XFillRectangle(
                        m_env->d, 
                        m_env->w, 
                        m_env->gc,
                        (j - INFINITE_FACTOR_2 - 1) * 10,
                        (i - INFINITE_FACTOR_2 - 1) * 10,
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
