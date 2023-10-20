#include "X11Window.h"
#include "cgol.h"
#include "utils.h"

#include <stdio.h>


X11Env*     env  = NULL;
CGOLMatrix* cgol = NULL;


/**
 * @brief  run game
 * @return int
 * @param  args is parsed args
*/
int Run(CGOLArgs* args)
{
    ttime_t begintime       = 0;
    volatile uint8_t* flags = &args->XEvtFlags;

    CGOL_begin_msg(args);

    while (CGOL_DO(*flags))
    {
        CGOL_X11_next_evt(args);

        switch (*flags)
        {
            // clear command registered
            case CGOL_CLEAR:
            {
                CGOL_clear_grid(cgol);
                CGOL_X11_clear(args);

                // clear it
                XEVENT_CLR(*flags, XEVENT_CLEAR);
                break;
            }

            // new seed command registered
            case CGOL_NEWSEED:
            {
                args->seed = CGOL_rand_seed();

                CGOL_newseed(cgol, args);
                CGOL_begin_msg(args);
                CGOL_X11_draw_grid(args);

                // clear it
                XEVENT_CLR(*flags, XEVENT_NEWSEED);
                break;
            }

            // expose event registered
            case CGOL_FORCE_REDRAW:
            {
                CGOL_X11_clear(args);
                CGOL_X11_draw_grid(args);
                break;
            }

            default:
            {
                // do next game cycle if it isn't suspended
                if (CGOL_CYCLE(*flags)) 
                {   
                    begintime = utime();

                    CGOL_algorithm(cgol);
                    CGOL_X11_clear(args);
                    CGOL_X11_draw_grid(args);

                    CGOL_adaptive_sleep(&begintime, args->rate);
                }
                break;
            }
        }
    }

    return EXIT_SUCCESS;
}


/**
 * @brief  something wont start, error!
 * @return int
 * @param  void
*/
int Failure(void)
{
    fprintf(stderr, "Failed to stat cgol :(\n");
    return EXIT_FAILURE;
}


/**
 * @brief  gcc destructor extension, it frees allocated data
 * @return void
 * @param  void
*/
void __attribute__((destructor)) destructor(void) 
{
    CGOL_release_grid(cgol);
    CGOL_X11_delete_env();

    printf("\nBye bye from CGOL, see you soon ;)\n");
    fflush(stdout);
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