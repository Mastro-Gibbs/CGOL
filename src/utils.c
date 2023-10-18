#include "utils.h"


#include "common.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

long value(char* strval)
{
    long value = -1;
    int  valid = 1;

    for (int j = 0; strval[j] != '\0'; j++) 
    {
        if (!isdigit(strval[j])) 
        {
            valid = 0;
            break;
        }
    }

    if (valid) value = atol(strval);

    return value;
}


void usage(void)
{
    printf("Usage: ./cgol [OPTION] [PARAM VALUE]\n\n"
           "PARAMS:\n"
           "-s -> grid seed\n"
           "-w -> window width\n"
           "-h -> window height\n"
           "-d -> default grid density\n"
           "-r -> grid update rate [1..10]\n\n"
           "OPTIONS\n"
           "-m -> max window size\n"
           "-b -> black mode\n"
           "-? -> help\n\n"
           "VALUES\n"
           "positive integers\n");
}

CGOLArgs ArgsErr(CGOLArgs* args)
{
    fprintf(stderr, "Error: option without value.\n");
    return *args;
}


void CGOL_begin_msg(CGOLArgs* args)
{
    printf(
            "Started Conway's Game Of Life using:\n"
            "-seed:    %lu\n"
            "-rate:    %u\n" 
            "-width:   %upx\n"
            "-height:  %upx\n"
            "-density: %u%%\n",
            args->seed,
            (args->rate == 999999) ? 1 : args->rate == 100000 ? 10 : ((1000000 - args->rate) / 100000),
            args->display.width,
            args->display.height,
            args->density
        );

    fflush(stdout);
}


CGOLArgs CGOL_parse_args(int argc, char** argv) 
{
    srand(time(NULL));

    size_t a = rand(), b = time(NULL);
    a = (a ^ 0xdeadbeef) + (b ^ 0xbeeff00d);
    a ^= (a >> 16);
    a *= 0x5a0849e7;
    a ^= (a >> 13);
    a *= 0x7afb6c3d;
    a ^= (a >> 16);

    CGOLArgs args = {a, 300000, {800, 600, 0, 0}, 11};

    if (1 == argc) return args;

    for (int i = 1; i < argc; ++i) 
    {
        if (strcmp(argv[i], "-?") == 0)
        {
            usage();
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            args.display.maxsize = 1;
            continue;
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            args.display.blackmode = 1;
            continue;
        }

        if (i + 1 >= argc) return ArgsErr(&args);

        else if (strcmp(argv[i], "-s") == 0) 
        {
            i++;
            size_t val = value(argv[i]);

            if (val != -1) args.seed = val;
            else return ArgsErr(&args);
        } 
        else if (strcmp(argv[i], "-w") == 0) 
        {
            i++;
            uint32_t val = (uint32_t) value(argv[i]);

            if (val != -1) args.display.width = val;
            else return ArgsErr(&args);
        } 
        else if (strcmp(argv[i], "-h") == 0) 
        {
            i++;
            uint32_t val = (uint32_t) value(argv[i]);

            if (val != -1) args.display.height = val;
            else return ArgsErr(&args);
        } 
        else if (strcmp(argv[i], "-d") == 0) 
        {
            i++;
            uint32_t val = (uint32_t) value(argv[i]);

            if (val <= 1)
                val = 1;
            else if (val >= 100)
                val = 100;

            if (val != -1) args.density = val;
            else return ArgsErr(&args);
        } 
        else if (strcmp(argv[i], "-r") == 0) 
        {
            i++;
            uint32_t val = (uint32_t) value(argv[i]);

            if (val != -1)
            {
                if (val <= 1)
                    val = 999999;
                else if (val >= 10)
                    val = 100000;
                else
                    val = 1000000 - (val * 100000);
                
                args.rate = val;
            }
            else return ArgsErr(&args);
        } 
        else return ArgsErr(&args);
    }

    return args;
}
