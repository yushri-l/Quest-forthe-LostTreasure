/*
 * Quest for the Lost Treasure
 * A console-based, turn-based adventure game written in C.
 *
 * SE1012 - Programming Methodology
 */

#include <stdio.h>

/* ---- Game constants (counts taken from the assignment spec) ---- */
#define GRID_SIZE     15   /* the map is a 15 x 15 grid           */
#define WALLS         30   /* random interior walls               */
#define TREASURES     12   /* treasure tiles                      */
#define HEALTH_PACKS   5   /* health packs                        */
#define KEYS           3   /* keys                                */
#define DOORS          3   /* locked doors                        */
#define TRAPS         10   /* hidden traps                        */

/* ---- Tile symbols ---- */
#define WALL     '#'
#define TREASURE 'T'
#define HEALTH   'H'
#define KEY      'K'
#define DOOR     'D'
#define EMPTY    ' '

/* ---- Global game state ---- */
char map[GRID_SIZE][GRID_SIZE];        /* the visible map                 */
int  hiddenTrap[GRID_SIZE][GRID_SIZE]; /* 1 where a hidden trap sits      */

int main(void)
{
    printf("=================================\n");
    printf("   Quest for the Lost Treasure\n");
    printf("=================================\n");

    return 0;
}
