#ifndef __CGOL_H__
#define __CGOL_H__

#include "common.h"
#include <stdlib.h>

CGOLMatrix* CGOL_init_grid(CGOLArgs* args);

void CGOL_clear_grid(CGOLMatrix* cgol);

void CGOL_newseed(CGOLMatrix* cgol, CGOLArgs* args);

void CGOL_release_grid(CGOLMatrix* cgol);

void CGOL_algorithm(CGOLMatrix* cgol);



#endif /* __CGOL_H__ */