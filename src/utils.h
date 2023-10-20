#ifndef __CGOL_UTILS_H__
#define __CGOL_UTILS_H__

#include "common.h"

/**
 * @brief  print on stdout new game setting
 * @return void
 * @param args is struct reference that hold cgol settings
*/
void CGOL_begin_msg(CGOLArgs* args);

/**
 * @brief generate a new rand seed 
 * @return seed
 * @param void
*/
size_t CGOL_rand_seed(void);

/**
 * @brief sleep function. it sleeps for 
 *        (1000000 / *rate)  - (utime() - *bt).
 * @return void
 * @param bt is reference to value returned from utime()
 * @param rate is tick rate per second or how many CGOL cycle you want for second
*/
void CGOL_adaptive_sleep(ttime_t* bt, uint8_t rate);

/**
 * @brief parse program arguments
 * @return instance of CGOLArgs
 * @param argc is arguments count
 * @param argv is arguments value count
*/
CGOLArgs CGOL_parse_args(int argc, char** argv);

/**
 * @brief  get the time in microseconds
 * @return ttime_t aka unsigned long long
 * @param  void
*/
ttime_t utime(void);


#endif /* __CGOL_UTILS_H__ */