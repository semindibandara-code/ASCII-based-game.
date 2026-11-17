#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "map.h"
#include "util.h"
#include "terminal.h"
#include "newSleep.h"
#include "ll.h"

/* rendering */

static void draw(const struct Map *m)
{
    int r, c, i;
    int inner_w;

    inner_w = (*m).cols;

    /* Clear screen. */
    /* printf("\x1b[2J\x1b[H"); */
    system("clear");

    /* top border */
    for (i = 0; i < inner_w + 2; i++) putchar('*');
    putchar('\n');

    for (r = 0; r < (*m).rows; r++)
    {
        putchar('*'); /* left border */
        for (c = 0; c < (*m).cols; c++)
        {
            int t;
            char ch;

            t = map_get(m, r, c);
            ch = tile_to_char(t);
            if (r == (*m).pr && c == (*m).pc) ch = 'P';

            /* background colors only for special tiles */
            apply_tile_color_begin(t);
            putchar(ch);
            apply_tile_color_end();
        }
        putchar('*'); /* right border */
        putchar('\n');
    }

    /* bottom border */
    for (i = 0; i < inner_w + 2; i++) putchar('*');
    putchar('\n');

    /* legend */
    printf("Press 'w' to move UP\n");
    printf("Press 's' to move DOWN\n");
    printf("Press 'a' to move LEFT\n");
    printf("Press 'd' to move RIGHT\n");
    printf("Press 'u' to UNDO\n");
}

/* rules / mechanics */

static int walkable(int t)
{
    if (t == T_WALL) return 0;
    if (t == T_TRAPDOOR) return 0;
    return 1;
}

/* remove all traps and trapdoors the first time you step on '@' */
static void trigger_trap(struct Map *m)
{
    int r, c;
    (*m).trap_active = 1;
    for (r = 0; r < (*m).rows; r++)
    {
        for (c = 0; c < (*m).cols; c++)
        {
            int t = map_get(m, r, c);
            if (t == T_TRAP || t == T_TRAPDOOR)
            {
                map_set(m, r, c, T_EMPTY);
            }
        }
    }
}

/* brute-force flood step from every existing water cell into empty neighbors */
static void flood_step(struct Map *m)
{
    int rows, cols, r, c;
    int *snap;
    size_t ncell, i;

    rows = (*m).rows;
    cols = (*m).cols;
    ncell = (size_t)rows * (size_t)cols;

    snap = (int*)malloc(ncell * sizeof(int));
    if (snap == NULL) return;

    for (i = 0; i < ncell; i++) snap[i] = (*m).grid[i];

    for (r = 0; r < rows; r++)
    {
        for (c = 0; c < cols; c++)
        {
            if (snap[r*cols + c] == T_WATER)
            {
                if (r > 0 && snap[(r-1)*cols + c] == T_EMPTY)
                    (*m).grid[(r-1)*cols + c] = T_WATER;
                if (r < rows-1 && snap[(r+1)*cols + c] == T_EMPTY)
                    (*m).grid[(r+1)*cols + c] = T_WATER;
                if (c > 0 && snap[r*cols + (c-1)] == T_EMPTY)
                    (*m).grid[r*cols + (c-1)] = T_WATER;
                if (c < cols-1 && snap[r*cols + (c+1)] == T_EMPTY)
                    (*m).grid[r*cols + (c+1)] = T_WATER;
            }
        }
    }

    free(snap);
}

/* UNDO state serialization */

struct StateHdr {
    int rows, cols;
    int pr, pc;
    int trap_active;
};

static int make_state_blob(const struct Map *m, void **out, size_t *out_sz)
{
    size_t ncell, sz;
    unsigned char *p;
    struct StateHdr hdr;

    ncell = (size_t)(*m).rows * (size_t)(*m).cols;
    sz = sizeof(struct StateHdr) + ncell * sizeof(int);

    p = (unsigned char*)malloc(sz);
    if (p == NULL) return 1;

    hdr.rows = (*m).rows;
    hdr.cols = (*m).cols;
    hdr.pr = (*m).pr;
    hdr.pc = (*m).pc;
    hdr.trap_active = (*m).trap_active;

    memcpy(p, &hdr, sizeof(hdr));
    memcpy(p + sizeof(hdr), (const void*)(*m).grid, ncell * sizeof(int));

    *out = (void*)p;
    *out_sz = sz;
    return 0;
}

static void restore_state_blob(struct Map *m, const void *blob, size_t sz)
{
    const unsigned char *p;
    const struct StateHdr *hdr;
    size_t ncell, expect;

    p = (const unsigned char*)blob;
    hdr = (const struct StateHdr*)p;

    ncell = (size_t)(*hdr).rows * (size_t)(*hdr).cols;
    expect = sizeof(struct StateHdr) + ncell * sizeof(int);

    if (sz != expect) return;
    if ((*hdr).rows != (*m).rows || (*hdr).cols != (*m).cols) return;

    (*m).pr = (*hdr).pr;
    (*m).pc = (*hdr).pc;
    (*m).trap_active = (*hdr).trap_active;

    memcpy((void*)(*m).grid, p + sizeof(struct StateHdr), ncell * sizeof(int));
}

/* main loop */

int game_loop(struct Map *m)
{
    int running, outcome;
    char ch = '\0';

    struct LLStack undo;

    running = 1;
    outcome = 2; /* 0=win, 1=lose, 2=quit */
    ll_init(&undo);

    while (running)
    {
        int t_here;
        int handled = 0;    /* set nonzero if processed input this frame (e.g., undo/quit) */
        int want_move = 0;
        int nr = (*m).pr, nc = (*m).pc;

        /* win/lose at frame start */
        t_here = map_get(m, (*m).pr, (*m).pc);
        if (t_here == T_GOAL) { outcome = 0; running = 0; }
        else if (t_here == T_WATER) { outcome = 1; running = 0; }

        if (running) {
            draw(m);

            /* input */
            disableBuffer();
            if (scanf(" %c", &ch) != 1) ch = 'q';
            enableBuffer();

            /* undo */
            if (!handled && ch == 'u') {
                void *blob = NULL; size_t bsz = 0;
                if (ll_pop_take(&undo, &blob, &bsz) == 0) {
                    restore_state_blob(m, blob, bsz);
                    free(blob);
                }
                newSleep(0.03f);
                handled = 1;
            }

            /* movement */
            if (!handled) {
                if (ch == 'w') { nr--; want_move = 1; }
                else if (ch == 's') { nr++; want_move = 1; }
                else if (ch == 'a') { nc--; want_move = 1; }
                else if (ch == 'd') { nc++; want_move = 1; }

                if (want_move &&
                    nr >= 0 && nr < (*m).rows && nc >= 0 && nc < (*m).cols) {

                    int t_next = map_get(m, nr, nc);
                    if (walkable(t_next)) {
                        /* UNDO checkpoint BEFORE mutation */
                        {
                            void *blob; size_t bsz;
                            if (make_state_blob(m, &blob, &bsz) == 0) {
                                (void)ll_push(&undo, blob, bsz);
                                free(blob);
                            }
                        }

                        (*m).pr = nr; (*m).pc = nc;

                        if (t_next == T_TRAP && (*m).trap_active == 0) {
                            trigger_trap(m);
                        }
                        if ((*m).trap_active) {
                            flood_step(m);
                            if (map_get(m, (*m).pr, (*m).pc) == T_WATER) {
                                outcome = 1; running = 0;
                            }
                        }
                    }
                }
                newSleep(0.05f);
            }
        }
    }

    /* final redraw only; main() prints the message */
    draw(m);

    /* clear remaining undo states */
    ll_clear(&undo);

    return outcome; /* single return */
}
