#ifndef MAP_H
#define MAP_H

struct Map {
    int rows, cols;
    int pr, pc;       /* player row/col */
    int trap_active;  /* 1 once stepped on '@' */
    int *grid;        /* rows*cols int tiles */
};

int map_load(const char *path, struct Map *m); /* return 0 on ok */
void map_free(struct Map *m);
int map_get(const struct Map *m, int r, int c);
void map_set(struct Map *m, int r, int c, int v);

#endif

