#ifndef __CGOL_H__
#define __CGOL_H__

#include "common.h"
#include <stdlib.h>

CGOLMatrix* CGOL_init_grid(const CGOLArgs* args);

void CGOL_release_grid(CGOLMatrix* grid);

void CGOL_algorithm(CGOLMatrix* grid);



#endif /* __CGOL_H__ */