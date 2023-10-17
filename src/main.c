#include "X11Window.h"
#include "cgol.h"
#include "utils.h"

#include <stdio.h>


int Run(CGOLMatrix* grid, CGOLArgs* args)
{
    volatile uint32_t exit    = 0;
    volatile uint32_t exposed = 0;
    volatile uint32_t evt     = 1;

    CGOL_begin_msg(args);

    while (!exit)
    {
        if (!evt && exposed) 
        {   
            CGOL_X11_draw_grid(grid);
            CGOL_algorithm(grid);
            usleep(args->rate);

            continue;    
        }

        CGOL_X11_next_evt(&evt, &exit, &exposed);
    }

    CGOL_release_grid(grid);
    CGOL_X11_delete_env();

    return 0;
}


int Failure(CGOLMatrix* grid)
{
    CGOL_release_grid(grid);
    CGOL_X11_delete_env();

    fprintf(stderr, "Failed to stat cgol :(\n");

    return 1;
}


int main(int argc, char** argv)
{   
    CGOLArgs    args = CGOL_parse_args(argc, argv);
    X11Env*     env  = CGOL_X11_create(&args.display, 1);
    CGOLMatrix* grid = CGOL_init_grid(&args);

    if (NULL == grid || NULL == env) 
        return Failure(grid);

    return Run(grid, &args);
}