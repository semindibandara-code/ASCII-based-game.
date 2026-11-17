#include <stdio.h>
#include "util.h"
#include "color.h"   /* setForeground/setBackground */  /* :contentReference[oaicite:10]{index=10} */

char tile_to_char(int t)
{
    char c;
    switch(t)
    {
        case T_WALL:     c='O'; break;
        case T_GOAL:     c='G'; break;
        case T_TRAP:     c='@'; break;
        case T_WATER:    c='~'; break;
        case T_TRAPDOOR: c='X'; break;		 
        case T_EMPTY:
       	default:         c=' '; break;
    }
    return c;
}

void apply_tile_color_begin(int t)
{
    /* colors: blue water, red trap, green goal; others no bg */
    if(t==T_WATER)
    {
        setBackground("blue");   /* :contentReference[oaicite:11]{index=11} */
        setForeground("white");
    }
    else if(t==T_TRAP)
    {
        setBackground("red");    /* :contentReference[oaicite:12]{index=12} */
        setForeground("white");
    }
    else if(t==T_GOAL)
    {
        setBackground("green");  /* :contentReference[oaicite:13]{index=13} */
        setForeground("black");
    }
}

void apply_tile_color_end(void)
{
    setForeground("reset");      /* :contentReference[oaicite:14]{index=14} */
    setBackground("reset");      /* :contentReference[oaicite:15]{index=15} */
}

