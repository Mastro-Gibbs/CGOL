#include "X11Window.h"
#include "cgol.h"
#include "utils.h"

#include <stdio.h>

X11Env*     env  = NULL;
CGOLMatrix* cgol = NULL;

void __attribute__((destructor)) destructor(void) 
{
    CGOL_release_grid(cgol);
    CGOL_X11_delete_env();
}


void ClearCGOL(XEvtArgs* xargs)
{
    CGOL_clear_grid(cgol);
    CGOL_X11_clear();
    xargs->clear = 0;
}


void NewCGOL(CGOLArgs* args)
{
    args->seed = CGOL_rand_seed();
    CGOL_newseed(cgol, args);
    CGOL_begin_msg(args);
    CGOL_X11_draw_grid(cgol);
    args->xargs.newseed = 0;
}


int Run(CGOLArgs* args)
{
    ttime_t begintime = 0;

    CGOL_begin_msg(args);

    while (!args->xargs.exit)
    {
        CGOL_X11_next_evt(args);

        if (args->xargs.exposed && args->xargs.suspend && args->xargs.clear)
        {
            ClearCGOL(&args->xargs);
        }

        else if (args->xargs.exposed && args->xargs.newseed)
        {
            NewCGOL(args);
        }

        else if (args->xargs.evt && args->xargs.exposed) 
        {
            CGOL_X11_clear();
            CGOL_X11_draw_grid(cgol);
        }

        else if (!args->xargs.evt && args->xargs.exposed && !args->xargs.suspend) 
        {   
            begintime = utime();

            CGOL_algorithm(cgol);
            CGOL_X11_clear();
            CGOL_X11_draw_grid(cgol);

            CGOL_adaptive_sleep(&begintime, &args->rate);
        }
    }

    return 0;
}


int Failure(void)
{
    fprintf(stderr, "Failed to stat cgol :(\n");
    return 1;
}


int main(int argc, char** argv)
{   
    CGOLArgs args = CGOL_parse_args(argc, argv);
             
    env  = CGOL_X11_create(&args.display);
    cgol = CGOL_init_grid(&args);

    if (NULL == cgol || NULL == env) 
        return Failure();

    return Run(&args);
}