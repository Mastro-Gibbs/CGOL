#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdlib.h>

#include "X11Window.h"


#if USE_ICON_LARGE == 1
  #include "ico/iconlarge.xbm"
  #define ICON_DATA   (_Xconst char*) iconlarge_bits
  #define ICON_WIDTH  iconlarge_width
  #define ICON_HEIGHT iconlarge_height
#else 
  #include "ico/icon.xbm"
  #define ICON_DATA   (_Xconst char*) icon_bits
  #define ICON_WIDTH  icon_width
  #define ICON_HEIGHT icon_height
#endif



/**
 * implementation for X11Env struct
*/
struct X11Env
{
    Display* d;
    Window   w;

    GC       gc;
    int      s;

	Atom wmDeleteMessage;

    uint32_t width;
    uint32_t height;
};


X11Env* x11env = NULL;


/**
 * @brief  set X windows hints (size and bitmap)
 * @return void
 * @param  void
*/
void X11_hints_and_protocols(void)
{
    XSizeHints* size_hints  = NULL; // size hints
    XWMHints*   pixmap_hint = NULL; // bitmap hints

    Pixmap icon_pixmap = XCreateBitmapFromData(x11env->d, 
                                               RootWindow(x11env->d, x11env->s), 
                                               ICON_DATA, 
                                               ICON_WIDTH, 
                                               ICON_HEIGHT);

    size_hints = XAllocSizeHints();

    if (NULL != size_hints)
    {
        size_hints->flags = PMinSize | PMaxSize;
        size_hints->min_width  = size_hints->max_width  = x11env->width;
        size_hints->min_height = size_hints->max_height = x11env->height;
        XSetWMNormalHints(x11env->d, x11env->w, size_hints);
        XFree(size_hints);
    }

    pixmap_hint = XAllocWMHints();

    if (NULL != pixmap_hint)
    {
        pixmap_hint->icon_pixmap = icon_pixmap;
        pixmap_hint->flags = IconPixmapHint;
        XSetWMHints(x11env->d, x11env->w, pixmap_hint);
        XFree(pixmap_hint);
    }

	// register interest in the delete window message
    x11env->wmDeleteMessage = XInternAtom(x11env->d, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(x11env->d, x11env->w, &x11env->wmDeleteMessage, 1);
}


void X11_colors(void)
{
	// set bg color
    XSetBackground(x11env->d, x11env->gc, BACKGROUND_COLOR);

	// set color for cells
    XSetForeground(x11env->d, x11env->gc, CELL_COLOR);
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

    // create a GC
    x11env->gc = XCreateGC(x11env->d, x11env->w, 0, NULL);

    // set window hints and protocols
    X11_hints_and_protocols(); 

	// parse and set colors
	X11_colors();

    // chose event listers
    XSelectInput(x11env->d, x11env->w, KeyPressMask | ExposureMask | ButtonPressMask);

    // set window name
    XStoreName(x11env->d, x11env->w, "CGOL: Conway's Game Of Life");

    XMapWindow(x11env->d, x11env->w);

    return x11env;
}


void X11_clear(CGOLArgs* args)
{
    // clear content
    XClearWindow(x11env->d, x11env->w); 

	// set bg color
    XSetWindowBackground(x11env->d, x11env->w, BACKGROUND_COLOR);

    // set color for grid
    XSetForeground(x11env->d, x11env->gc, GRID_COLOR);

    // draw grid
    for (uint32_t i = 0; i < x11env->width; i+=args->gridsize)
    {
        XDrawLine(x11env->d, x11env->w, x11env->gc, 0, i, x11env->width, i);
        XDrawLine(x11env->d, x11env->w, x11env->gc, i, 0, i, x11env->height);
    }

    // restore color for cells
    XSetForeground(x11env->d, x11env->gc, CELL_COLOR);

    // flush the display
    XFlush(x11env->d);
}

#include <stdio.h>
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
            if (args->xflags & XEVENT_SUSPEND)
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
                XEVENT_CHG(args->xflags, XEVENT_SUSPEND);
                break;

            // quit game
            case KEY_Q:
                XEVENT_SET(args->xflags, XEVENT_EXIT);
                break;

            // new game
            case KEY_N: 
                XEVENT_SET(args->xflags, XEVENT_NEWGAME);
                break;

            // clear game
            case KEY_C:
                if (args->xflags & XEVENT_SUSPEND)
                    XEVENT_SET(args->xflags, XEVENT_CLEAR);
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
            XEVENT_SET(args->xflags, XEVENT_EXPOSED);
            break;

		// check client messages
        case ClientMessage:
			// window X bnt pressed
            if (xevt.xclient.data.l[0] == x11env->wmDeleteMessage)
			{
				XEVENT_SET(args->xflags, XEVENT_EXIT);
			}
			break;
        
        default:
            break;
        }

        // an event appear
        XEVENT_SET(args->xflags, XEVENT_EVT);
    }
    // no event in queue
    else XEVENT_CLR(args->xflags, XEVENT_EVT);
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
