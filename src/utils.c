#include "utils.h"


#include "common.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>


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

void ArgsErr(void)
{
    fprintf(stderr, "Error: param without/invalid value or unknown option.\n");
    exit(EXIT_FAILURE);
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

    if (!args->xargs.exposed) runtime_commands();

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


void CGOL_adaptive_sleep(ttime_t* bt, uint32_t* rate)
{
    time_t  sleeptime = 999999 / *rate;

    usleep(sleeptime - (utime() - *bt));
}


CGOLArgs init_CGOLArgs(void)
{
    srand(time(NULL));
    size_t randseed = CGOL_rand_seed();

    CGOLArgs args = 
    {
        .seed    = randseed, 
        .rate    = 4, 
        .density = 11, 
        .display = 
        {
            .width   = 800, 
            .height  = 600, 
            .maxsize = 0
        }, 

        .xargs = 
        {
            .exposed = 0, 
            .evt     = 1, 
            .suspend = 0, 
            .clear   = 0, 
            .newseed = 0, 
            .exit    = 0
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

        if (strcmp(param, "--help") == 0)
        {
            usage();
            exit(EXIT_SUCCESS);
        }

        else if (strcmp(param, "-m") == 0)
        {
            args.display.maxsize = 1;
            continue;
        }

        else if (strcmp(param, "-e") == 0)
        {
            args.xargs.clear   = 1;
            args.xargs.suspend = 1;
            continue;
        }

        if (i + 1 >= argc) continue;

        ++i;
        strval = argv[i];
        val    = 0;

        if (strcmp(param, "-s") == 0) 
        {
            if (value(strval, &val)) args.seed = (size_t) val;
        } 

        else if (strcmp(param, "-w") == 0) 
        {
            if (value(strval, &val))
            {
                if (val < 800)
                {
                    val = 800;
                    printf("Value too low for %s: nomalized to %lu\n", param, val);
                }
                
                args.display.width = (uint32_t) val;
            }
        } 

        else if (strcmp(param, "-h") == 0) 
        {
            if (value(strval, &val))
            {
                if (val < 600)
                {
                    val = 600;
                    printf("Value too low for %s: nomalized to %lu\n", param, val);
                }
                
                args.display.height = (uint32_t) val;
            }
        } 

        else if (strcmp(param, "-d") == 0) 
        {
            if (value(strval, &val))
            {
                if (val < 5)
                    args.density = 5;
                else if (val > 50)
                    args.density = 50;
                else
                    args.density = val;

                if (val < 5 || val > 50)
                    printf("Value too low for %s: nomalized to %u\n", param, args.density);
            }
        } 

        else if (strcmp(param, "-u") == 0) 
        {
            if (value(strval, &val))
            {
                if (val > 50)
                    args.rate = 50;
                else
                    args.rate = val;

                if (val > 50)
                    printf("Value too low for %s: nomalized to %u\n", param, args.rate);
            }
        } 

        else
        {
            val = 0;
        }
    }

    if (val == 0)
    {
        fprintf(stderr, "Error: option '%s' without/invalid value or unknown.\n", param);
        exit(EXIT_FAILURE);
    }

    if (args.xargs.clear && args.xargs.suspend)
    {
        args.seed    = 0;
        args.density = 0;
    }

    return args;
}
