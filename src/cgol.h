#ifndef __CGOL_H__
#define __CGOL_H__

#include "common.h"
#include <stdlib.h>

/**
 * @brief  initialize the matrixs for CGOL
 * @return CGOLMatrix* on success (needs to be freed) or NULL on error
 * @param  args parsed arguments
*/
CGOLMatrix* CGOL_init_grid(CGOLArgs* args);

/**
 * @brief  erase current both matrix
 * @return void
 * @param  cgol is CGOLMatrix reference
*/
void CGOL_clear_grid(CGOLMatrix* cgol);

/**
 * @brief  prepare a runtime new rand configuration based on args
 * @return void
 * @param  cgol is current configuration/status reference
 * @param  args is new args reference
*/
void CGOL_newseed(CGOLMatrix* cgol, CGOLArgs* args);

/**
 * @brief  deallocate CGOL buffers
 * @return void
 * @param  cgol is CGOLMatrix reference to be deallocated
*/
void CGOL_release_grid(CGOLMatrix* cgol);

/**
 * @brief  compute a new game cycle (it's the core!)
 * @return void
 * @param  cgol is s CGOLMatrix reference
*/
void CGOL_algorithm(CGOLMatrix* cgol);



#endif /* __CGOL_H__ */