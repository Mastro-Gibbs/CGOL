#ifndef __X11WINDOW_H__
#define __X11WINDOW_H__

#include "common.h"
#include <inttypes.h>


X11Env* CGOL_X11_create(X11Display* display,
                       const uint8_t set);

void CGOL_X11_clear(void);

void CGOL_X11_next_evt(CGOLArgs* args);

void CGOL_X11_draw_grid(const CGOLMatrix* cgol);

void CGOL_X11_delete_env(void);


#endif /* __X11WINDOW_H__ */