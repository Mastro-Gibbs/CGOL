#ifndef __CGOL_UTILS_H__
#define __CGOL_UTILS_H__

#include "common.h"

/**
 * @brief  print on stdout new game setting
 * @return void
 * @param args is struct reference that hold cgol settings
*/
void begin_msg(CGOLArgs* args);

/**
 * @brief generate a new rand seed 
 * @return seed
 * @param void
*/
size_t rand_seed(void);

/**
 * @brief sleep function. it sleeps for 
 *        (1000000 / rate)  - (utime() - begintime).
 * @return void
 * @param args is struct reference that hold cgol settings
*/
void adaptive_sleep(CGOLArgs* args);

/**
 * @brief parse program arguments
 * @return instance of CGOLArgs
 * @param argc is arguments count
 * @param argv is arguments value count
*/
CGOLArgs parse_args(int argc, char** argv);

/**
 * @brief  get the time in microseconds
 * @return ttime_t aka unsigned long long
 * @param  void
*/
ttime_t utime(void);


#endif /* __CGOL_UTILS_H__ */