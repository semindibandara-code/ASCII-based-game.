#ifndef UTIL_H
#define UTIL_H

enum Tile { T_EMPTY=0, T_WALL=2, T_GOAL=3, T_TRAP=4, T_WATER=5, T_TRAPDOOR=6 };

char tile_to_char(int t);
void apply_tile_color_begin(int t);   /* uses color.c */
void apply_tile_color_end(void);

#endif

