#include <stdio.h>
#include "map.h"
#include "game.h"

int main(int argc, char **argv)
{
    int ret = 0;
    struct Map m;
    int rc;

    /* argument check */
    if (argc < 2) {
        printf("usage: %s <mapfile>\n", argv[0]);
        ret = 1;
    } else {
        /* load map (map_load prints specific error messages on failure) */
        rc = map_load(argv[1], &m);
        if (rc != 0) {
            /* map_load already printed an error such as:
               - cannot open file
               - bad first line
               - grid incomplete, etc. */
            ret = 2;
        } else {
            /* run the game; game_loop should NOT print the final message */
            {
                int outcome = game_loop(&m);
                if (outcome == 0) {
                    printf("You reached the goal. You win!\n");
                    ret = 0;
                } else if (outcome == 1) {
                    printf("Water got you. You lose.\n");
                    ret = 0; /* non-error run, just a lose state */
                } else {
                    printf("Quit. Bye.\n");
                    ret = 0; /* quitting isn't an error */
                }
            }
            map_free(&m);
        }
    }

    return ret; /* single return in main */
}

