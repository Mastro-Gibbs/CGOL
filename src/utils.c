#include "utils.h"


#include "common.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>


/**
 * @brief  cast string to base 10 number. only positive number are valid
 * @return 0 on error, 1 otherwise
 * @param  strval is string
 * @param  intval is reference into store parsed number
*/
uint8_t value(char* strval, size_t* intval)
{
    for (int j = 0; strval[j] != '\0'; j++) 
    {
        if (!isdigit(strval[j])) return 0;
    }

    if (atol(strval) < 1) return 0;

    *intval = strtoul(strval, NULL, 10);

    return 1;
}


void runtime_commands(void)
{
    printf(
           "\n"
           "RUNTIME COMMANDS\n"
           "-KeyBoard:\n"
           "q -> quit\n"
           "s -> suspend/run execution\n"
           "z -> decrease update rate\n"
           "x -> increase update rate\n"
           "c -> clear all, works if execution is suspended\n"
           "n -> start new random seed\n"
           "-Mouse:\n"
           "left click -> toogle cell, works if execution is suspended\n"
        );
}


void usage(void)
{
    printf("Usage: ./cgol [OPTION] [PARAM VALUE]\n"
           "\n"
           "PARAMS:\n"
           "-s -> grid seed\n"
           "-w -> window width\n"
           "-h -> window height\n"
           "-d -> default grid density [5..50]\n"
           "-r -> grid update rate [1..50]\n"
           "-c -> cell size\n"
           "\n"
           "VALUES\n"
           "positive integers\n"
           "\n"
           "OPTIONS\n"
           "-m     -> max window size\n"
           "-e     -> start with editor mode on\n"
           "--help -> show this\n"
        );
    
    runtime_commands();
}


void CGOL_begin_msg(CGOLArgs* args)
{
    printf(
            "\n"
            "Started Conway's Game Of Life using:\n"
            "-seed:    %lu\n"
            "-width:   %u px\n"
            "-height:  %u px\n"
            "-density: %u %%\n"
            "-updates: %u per second\n",
            args->seed,
            args->display.width,
            args->display.height,
            args->density,
            args->rate
        );

    if (0 == (args->XEvtFlags & XEVENT_EXPOSED)) runtime_commands();

    fflush(stdout);
}


size_t CGOL_rand_seed(void)
{
    size_t a = rand(), b = time(NULL);
    a = (a ^ 0xdeadbeef) + (b ^ 0xbeeff00d);
    a ^= (a >> 16);
    a *= 0x5a0849e7;
    a ^= (a >> 13);
    a *= 0x7afb6c3d;
    a ^= (a >> 16);

    return a;
}



ttime_t utime(void) 
{
    struct timeval tempo;
    gettimeofday(&tempo, NULL);
    return ((unsigned long long)tempo.tv_sec) * 1000000 + tempo.tv_usec;
}


void CGOL_adaptive_sleep(ttime_t* bt, uint8_t rate)
{
    time_t  sleeptime = MAX_SLEEP_TIME / rate;

    usleep(sleeptime - (utime() - *bt));
}

/**
 * @brief  init default CGOL args
 * @return CGOLArgs object
 * @param  void
*/
CGOLArgs init_CGOLArgs(void)
{
    srand(time(NULL));
    size_t randseed = CGOL_rand_seed();

    CGOLArgs args = 
    {
        .seed      = randseed, 
        .rate      = DEFAULT_RATE, 
        .density   = DEFAULT_DENSITY,
        .gridsize  = DEFAULT_GRID_SIZE, 
        .cellsize  = DEFAULT_CELL_SIZE, 
        .XEvtFlags = DEFAULT_XEVENT_FLAGS,

        .display = 
        {
            .width   = X11WND_DEFAULT_WIDTH, 
            .height  = X11WND_DEFAULT_HEIGHT, 
            .maxsize = 0
        }, 

        .cgol = NULL
    };

    return args;
}


CGOLArgs CGOL_parse_args(int argc, char** argv) 
{
    CGOLArgs args = init_CGOLArgs();
    size_t   val  = 1;

    char*    param;
    char*    strval;

    if (1 == argc) return args;

    for (int i = 1; i < argc && val > 0; ++i) 
    {
        param = argv[i];

        // help
        if (strcmp(param, "--help") == 0)
        {
            usage();
            exit(EXIT_SUCCESS);
        }

        // window max size
        else if (strcmp(param, "-m") == 0)
        {
            args.display.maxsize = 1;
            continue;
        }

        // editor mode
        else if (strcmp(param, "-e") == 0)
        {
            XEVENT_SET(args.XEvtFlags, XEVENT_CLEAR);
            XEVENT_SET(args.XEvtFlags, XEVENT_SUSPEND);
            continue;
        }

        if (i + 1 >= argc) continue;

        ++i;
        strval = argv[i];
        val    = 0;

        // seed
        if (strcmp(param, "-s") == 0) 
        {
            if (value(strval, &val)) args.seed = (size_t) val;
        } 

        // cell size
        else if (strcmp(param, "-c") == 0) 
        {
            if (value(strval, &val))
            {
                if (val < DEFAULT_MIN_GRID_SIZE)
                    args.gridsize = DEFAULT_MIN_GRID_SIZE;
                else if (val > DEFAULT_MAX_GRID_SIZE)
                    args.gridsize = DEFAULT_MAX_GRID_SIZE;
                else
                    args.gridsize = val;

                if (val < DEFAULT_MIN_GRID_SIZE || val > DEFAULT_MAX_GRID_SIZE)
                    printf("Value off range for option '%s': nomalized to %u\n", param, args.gridsize);
                
                args.cellsize = (uint8_t) args.gridsize - 1;
            }
        }

        // width
        else if (strcmp(param, "-w") == 0) 
        {
            if (value(strval, &val))
            {
                if (val < X11WND_DEFAULT_WIDTH)
                {
                    val = X11WND_DEFAULT_WIDTH;
                    printf("Value too low for option '%s': nomalized to %lu\n", param, val);
                }
                
                args.display.width = (uint8_t) val;
            }
        } 

        // height
        else if (strcmp(param, "-h") == 0) 
        {
            if (value(strval, &val))
            {
                if (val < X11WND_DEFAULT_HEIGHT)
                {
                    val = X11WND_DEFAULT_HEIGHT;
                    printf("Value too low for option '%s': nomalized to %lu\n", param, val);
                }
                
                args.display.height = (uint8_t) val;
            }
        } 

        // density
        else if (strcmp(param, "-d") == 0) 
        {
            if (value(strval, &val))
            {
                if (val < DEFAULT_MIN_DENSITY)
                    args.density = DEFAULT_MIN_DENSITY;
                else if (val > DEFAULT_MAX_DENSITY)
                    args.density = DEFAULT_MAX_DENSITY;
                else
                    args.density = val;

                if (val < DEFAULT_MIN_DENSITY || val > DEFAULT_MAX_DENSITY)
                    printf("Value off range for option '%s': nomalized to %u\n", param, args.density);
            }
        } 

        // rate
        else if (strcmp(param, "-u") == 0) 
        {
            if (value(strval, &val))
            {
                if (val > DEFAULT_MAX_RATE)
                    args.rate = DEFAULT_MAX_RATE;
                else
                    args.rate = val;

                if (val > DEFAULT_MAX_RATE)
                    printf("Value off range for option '%s': nomalized to %u\n", param, args.rate);
            }
        } 

        // error
        else
        {
            val = 0;
        }
    }

    // argument error case
    if (val == 0) 
    {
        fprintf(stderr, "Error: option '%s' without/invalid value or unknown.\n", param);
        exit(EXIT_FAILURE);
    }

    // defaults for -e option
    if (args.XEvtFlags & CGOL_EDITOR_MODE) 
    {
        args.seed    = 0;
        args.density = 0;
    }

    // re-check rate if gridsize is less then DEFAULT_GRID_SIZE
    if (args.rate > DEFAULT_MAX_RATE_2 && args.gridsize < DEFAULT_GRID_SIZE)
    {
        printf("Value too high for option '-c' with cell size %u: nomalized to %u\n", args.gridsize, DEFAULT_MAX_RATE_2);
        args.rate = DEFAULT_MAX_RATE_2;
    }

    return args;
}
