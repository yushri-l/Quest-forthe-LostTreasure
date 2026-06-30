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

/*
 * initializeMap
 * Clears both arrays to their empty state, then draws the border walls
 * around the outermost ring of the grid.
 */
void initializeMap(void)
{
    int row, col;

    /* Fill every cell as empty and clear all hidden traps. */
    for (row = 0; row < GRID_SIZE; row++)
    {
        for (col = 0; col < GRID_SIZE; col++)
        {
            map[row][col] = EMPTY;
            hiddenTrap[row][col] = 0;
        }
    }

    /* Draw the border walls on the outer ring. */
    for (row = 0; row < GRID_SIZE; row++)
    {
        map[row][0] = WALL;                 /* left column  */
        map[row][GRID_SIZE - 1] = WALL;     /* right column */
    }
    for (col = 0; col < GRID_SIZE; col++)
    {
        map[0][col] = WALL;                 /* top row      */
        map[GRID_SIZE - 1][col] = WALL;     /* bottom row   */
    }
}

/*
 * printMap
 * Renders the visible grid to the console. Each cell is printed with a
 * trailing space so the square map is easy to read.
 */
void printMap(void)
{
    int row, col;

    for (row = 0; row < GRID_SIZE; row++)
    {
        for (col = 0; col < GRID_SIZE; col++)
        {
            printf("%c ", map[row][col]);
        }
        printf("\n");
    }
}

int main(void)
{
    printf("=================================\n");
    printf("   Quest for the Lost Treasure\n");
    printf("=================================\n");

    initializeMap();
    printMap();

    return 0;
}
