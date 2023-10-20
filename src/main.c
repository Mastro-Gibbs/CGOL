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
    volatile uint8_t* flags = &args->XEvtFlags;

    begin_msg(args);

    while (CGOL_DO(*flags))
    {
        X11_next_evt(args);

        switch (*flags)
        {
            // clear command registered
            case CGOL_CLEAR:
            {
                CGOL_clear(cgol);
                X11_clear(args);

                // clear it
                XEVENT_CLR(*flags, XEVENT_CLEAR);
                break;
            }

            // new seed command registered
            case CGOL_NEWGAME:
            {
                args->btime = utime();
                args->seed  = rand_seed();

                CGOL_newgame(cgol, args);
                X11_draw(args);

                adaptive_sleep(args);

                // clear it
                XEVENT_CLR(*flags, XEVENT_NEWGAME);
                break;
            }

            // expose event registered
            case CGOL_FORCE_REDRAW:
            {
                X11_draw(args);
                break;
            }

            default:
            {
                // do next game cycle if it isn't suspended
                if (CGOL_CYCLE(*flags)) 
                {   
                    args->btime = utime();

                    CGOL_algorithm(cgol);
                    X11_draw(args);

                    adaptive_sleep(args);
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
    CGOL_release(cgol);
    X11_release();

    printf("\nBye bye from CGOL, see you soon ;)\n");
    fflush(stdout);
}


int main(int argc, char** argv)
{   
    CGOLArgs args = parse_args(argc, argv);
             
    env  = X11_create(&args.display);
    cgol = CGOL_init(&args);

    if (NULL == cgol || NULL == env) 
        return Failure();

    return Run(&args);
}