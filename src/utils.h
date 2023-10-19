#ifndef __CGOL_UTILS_H__
#define __CGOL_UTILS_H__

#include "common.h"

void CGOL_begin_msg(CGOLArgs* args);

size_t CGOL_rand_seed(void);

void CGOL_adaptive_sleep(ttime_t* bt, uint32_t* rate);

CGOLArgs CGOL_parse_args(int argc, char** argv);

ttime_t utime(void);


#endif /* __CGOL_UTILS_H__ */