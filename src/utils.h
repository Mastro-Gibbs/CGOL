#ifndef __CGOL_UTILS_H__
#define __CGOL_UTILS_H__

#include "common.h"

void CGOL_begin_msg(CGOLArgs* args);

size_t CGOL_rand_seed(void);

void CGOL_pause(uint32_t times);

CGOLArgs CGOL_parse_args(int argc, char** argv);


#endif /* __CGOL_UTILS_H__ */