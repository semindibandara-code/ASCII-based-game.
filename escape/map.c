#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "util.h"

/* linear index helper */
static int idx(const struct Map *m, int r, int c)
{
    return r * (*m).cols + c;
}

/* Load the map from text file. Returns 0 on success, nonzero on error.
   On error, prints a clear message (so main() doesn’t need to duplicate it). */
int map_load(const char *path, struct Map *m)
{
    FILE *fp = NULL;
    int rr, cc;
    int code = 0;
    int foundP = 0;
    int status = 0;  /* 0 ok; nonzero = error */

    /* initialize struct to a safe state */
    (*m).rows = 0;
    (*m).cols = 0;
    (*m).pr = 0;
    (*m).pc = 0;
    (*m).trap_active = 0;
    (*m).grid = NULL;

    /* open file */
    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("error: cannot open '%s'\n", path);
        status = 1;
    }

    /* read header: rows cols */
    if (status == 0) {
        if (fscanf(fp, "%d %d", &(*m).rows, &(*m).cols) != 2) {
            printf("error: first line must be '<rows> <cols>'\n");
            status = 2;
        } else if ((*m).rows <= 0 || (*m).cols <= 0) {
            printf("error: rows/cols must be positive (got %d %d)\n", (*m).rows, (*m).cols);
            status = 3;
        }
    }

    /* allocate grid */
    if (status == 0) {
        (*m).grid = (int*)malloc((size_t)(*m).rows * (size_t)(*m).cols * sizeof(int));
        if ((*m).grid == NULL) {
            printf("error: out of memory for grid (%d x %d)\n", (*m).rows, (*m).cols);
            status = 4;
        }
    }

    /* read and translate the grid (0..6) */
    if (status == 0) {
        for (rr = 0; rr < (*m).rows; rr++) {
            for (cc = 0; cc < (*m).cols; cc++) {

                if (fscanf(fp, "%d", &code) != 1) {
                    printf("error: grid incomplete at row %d col %d\n", rr, cc);
                    status = 5;
                    break;
                }

                /* Translate your file codes to internal tile enums (util.h) */
                if (code == 0)               (*m).grid[idx(m, rr, cc)] = T_EMPTY;
                else if (code == 1)          (*m).grid[idx(m, rr, cc)] = T_WALL;
                else if (code == 2)          (*m).grid[idx(m, rr, cc)] = T_WATER;
                else if (code == 3)          (*m).grid[idx(m, rr, cc)] = T_TRAPDOOR;
                else if (code == 4)          (*m).grid[idx(m, rr, cc)] = T_TRAP;
                else if (code == 5) {
                    /* Player: record position and make the underlying tile empty */
                    (*m).pr = rr; (*m).pc = cc; foundP = 1;
                    (*m).grid[idx(m, rr, cc)] = T_EMPTY;
                }
                else if (code == 6)          (*m).grid[idx(m, rr, cc)] = T_GOAL;
                else                          (*m).grid[idx(m, rr, cc)] = T_EMPTY; /* fallback */
            }
            if (status != 0) {
                break;
            }
        }
    }

    /* done reading file */
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }

    /* must have exactly one player */
    if (status == 0 && !foundP) {
        printf("warning: no player '5' found in map; defaulting to (0,0)\n");
        (*m).pr = 0;
        (*m).pc = 0;
    }

    return status;  /* single return */
}

/* free grid memory */
void map_free(struct Map *m)
{
    if ((*m).grid != NULL) {
        free((*m).grid);
        (*m).grid = NULL;
    }
}

/* accessors */
int map_get(const struct Map *m, int r, int c)
{
    return (*m).grid[idx(m, r, c)];
}

void map_set(struct Map *m, int r, int c, int v)
{
    (*m).grid[idx(m, r, c)] = v;
}

