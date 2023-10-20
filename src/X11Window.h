#ifndef __X11WINDOW_H__
#define __X11WINDOW_H__

#include "common.h"
#include <inttypes.h>

/**
 * @brief  create X window
 * @return X11Env* on success (used to check if it was successful),
 *         NULL otherwise
 * @param  display is window setting
*/
X11Env* CGOL_X11_create(X11Display* display);

/**
 * @brief  clear X window content, generate a new grid
 * @return void
 * @param  args parsed arguments
*/
void CGOL_X11_clear(CGOLArgs* args);

/**
 * @brief  handle incoming X event
 * @return void
 * @param[in out] args hold event information
*/
void CGOL_X11_next_evt(CGOLArgs* args);

/**
 * @brief  draw new computed CGOL life cycle 
 * @return void
 * @param  args parsed arguments
*/
void CGOL_X11_draw_grid(CGOLArgs* args);

/**
 * @brief  deallocate X window
 * @return void
 * @param  void
*/
void CGOL_X11_delete_env(void);


#endif /* __X11WINDOW_H__ */