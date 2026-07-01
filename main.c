/*
 * Quest for the Lost Treasure
 * A console-based, turn-based adventure game written in C.
 *
 * SE1012 - Programming Methodology
 */

#include <stdio.h>
#include <stdlib.h>   /* rand, srand      */
#include <time.h>     /* time             */
#include <string.h>   /* strlen, strcspn  */
#include <ctype.h>    /* toupper          */

/* ---- Game constants (counts taken from the assignment spec) ---- */
#define GRID_SIZE     15   /* the map is a 15 x 15 grid           */
#define MAX_PLAYERS    2   /* Part A: exactly two players          */
#define START_HEALTH 100   /* every player starts with 100 HP      */
#define WALLS         30   /* random interior walls               */
#define TREASURES     12   /* treasure tiles                      */
#define HEALTH_PACKS   5   /* health packs                        */
#define KEYS           3   /* keys                                */
#define DOORS          3   /* locked doors                        */
#define TRAPS         10   /* hidden traps                        */

/* ---- Tile effect values ---- */
#define TREASURE_SCORE  10   /* score gained per treasure           */
#define HEALTH_RESTORE  20   /* HP restored per health pack         */
#define TRAP_DAMAGE     20   /* HP lost when a trap is triggered     */

/* ---- Tile symbols ---- */
#define WALL     '#'
#define TREASURE 'T'
#define HEALTH   'H'
#define KEY      'K'
#define DOOR     'D'
#define EMPTY    ' '

/*
 * Player
 * Holds everything we track for a single player. Position is stored as
 * (x, y) where x is the row and y is the column of the map grid.
 */
typedef struct
{
    char name[20];   /* player's name                         */
    int  x;          /* current row on the map                */
    int  y;          /* current column on the map             */
    int  score;      /* points collected from treasures/bonus */
    int  health;     /* hit points, starts at START_HEALTH    */
    int  keys;       /* keys held in inventory                */
    char symbol;     /* map symbol: '1' or '2'                */
} Player;

/* ---- Global game state ---- */
char   map[GRID_SIZE][GRID_SIZE];        /* the visible map              */
int    hiddenTrap[GRID_SIZE][GRID_SIZE]; /* 1 where a hidden trap sits   */
Player players[MAX_PLAYERS];             /* the players in the game      */
int    playerCount = MAX_PLAYERS;        /* how many players are active  */

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
 * placeOnRandomEmpty
 * Picks a random empty interior cell and writes the given symbol there.
 * Shared by the item-placement helpers so the search logic lives in one place.
 */
void placeOnRandomEmpty(char symbol)
{
    int row, col;

    do
    {
        row = 1 + rand() % (GRID_SIZE - 2);   /* interior rows 1..13 */
        col = 1 + rand() % (GRID_SIZE - 2);   /* interior cols 1..13 */
    } while (map[row][col] != EMPTY);

    map[row][col] = symbol;
}

/*
 * placeWalls
 * Randomly scatters WALLS (30) interior wall tiles on empty cells.
 */
void placeWalls(void)
{
    int i;

    for (i = 0; i < WALLS; i++)
    {
        placeOnRandomEmpty(WALL);
    }
}

/*
 * placeTreasures
 * Randomly scatters TREASURES (12) treasure tiles on empty cells.
 */
void placeTreasures(void)
{
    int i;

    for (i = 0; i < TREASURES; i++)
    {
        placeOnRandomEmpty(TREASURE);
    }
}

/*
 * placeHealthPacks
 * Randomly scatters HEALTH_PACKS (5) health-pack tiles on empty cells.
 */
void placeHealthPacks(void)
{
    int i;

    for (i = 0; i < HEALTH_PACKS; i++)
    {
        placeOnRandomEmpty(HEALTH);
    }
}

/*
 * placeKeys
 * Randomly scatters KEYS (3) key tiles on empty cells.
 */
void placeKeys(void)
{
    int i;

    for (i = 0; i < KEYS; i++)
    {
        placeOnRandomEmpty(KEY);
    }
}

/*
 * placeDoors
 * Randomly scatters DOORS (3) locked-door tiles on empty cells.
 */
void placeDoors(void)
{
    int i;

    for (i = 0; i < DOORS; i++)
    {
        placeOnRandomEmpty(DOOR);
    }
}

/*
 * placeTraps
 * Marks TRAPS (10) cells in the hidden trap array only. A trap always sits
 * on an empty-looking tile and is never written to the visible map, so
 * players cannot see it until they step on it.
 */
void placeTraps(void)
{
    int i, row, col;

    for (i = 0; i < TRAPS; i++)
    {
        do
        {
            row = 1 + rand() % (GRID_SIZE - 2);
            col = 1 + rand() % (GRID_SIZE - 2);
        } while (map[row][col] != EMPTY || hiddenTrap[row][col] != 0);

        hiddenTrap[row][col] = 1;
    }
}

/*
 * placePlayers
 * Sets up each active player's starting stats (name, symbol, health, score,
 * keys) and drops them on a random empty tile. Trap tiles are avoided so no
 * one starts standing on a hidden trap.
 */
void placePlayers(void)
{
    int i, row, col;

    for (i = 0; i < playerCount; i++)
    {
        /* Starting stats. */
        sprintf(players[i].name, "Player %d", i + 1);
        players[i].symbol = '1' + i;          /* '1', '2', ...            */
        players[i].health = START_HEALTH;
        players[i].score  = 0;
        players[i].keys   = 0;

        /* Find a free tile with no hidden trap under it. */
        do
        {
            row = 1 + rand() % (GRID_SIZE - 2);
            col = 1 + rand() % (GRID_SIZE - 2);
        } while (map[row][col] != EMPTY || hiddenTrap[row][col] != 0);

        players[i].x = row;
        players[i].y = col;
        map[row][col] = players[i].symbol;
    }
}

/*
 * processTile
 * Applies the effects of the tile the given player is now standing on. The
 * checks run in the order required by the spec: hidden trap, then treasure,
 * then health pack, then key. Collected tiles are cleared from the map.
 */
void processTile(int index)
{
    int x = players[index].x;
    int y = players[index].y;

    /* 1. Hidden trap: deal damage once, then disarm it. */
    if (hiddenTrap[x][y])
    {
        players[index].health -= TRAP_DAMAGE;
        if (players[index].health < 0)
        {
            players[index].health = 0;
        }
        hiddenTrap[x][y] = 0;
    }

    /* 2. Treasure: add score and remove it. */
    if (map[x][y] == TREASURE)
    {
        players[index].score += TREASURE_SCORE;
        map[x][y] = EMPTY;
    }

    /* 3. Health pack: restore HP (capped) and remove it. */
    if (map[x][y] == HEALTH)
    {
        players[index].health += HEALTH_RESTORE;
        if (players[index].health > START_HEALTH)
        {
            players[index].health = START_HEALTH;
        }
        map[x][y] = EMPTY;
    }

    /* 4. Key: add to inventory and remove it. */
    if (map[x][y] == KEY)
    {
        players[index].keys += 1;
        map[x][y] = EMPTY;
    }
}

/*
 * isValidMove
 * Returns 1 if (x, y) is inside the grid and is not a wall, otherwise 0.
 * Door handling (which needs a key) is done separately in movePlayer.
 */
int isValidMove(int x, int y)
{
    /* Out of bounds? */
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE)
    {
        return 0;
    }

    /* A wall blocks the move. */
    if (map[x][y] == WALL)
    {
        return 0;
    }

    return 1;
}

/*
 * remainingTreasures
 * Scans the map and returns how many treasure tiles are still uncollected.
 */
int remainingTreasures(void)
{
    int row, col, count = 0;

    for (row = 0; row < GRID_SIZE; row++)
    {
        for (col = 0; col < GRID_SIZE; col++)
        {
            if (map[row][col] == TREASURE)
            {
                count++;
            }
        }
    }
    return count;
}

/*
 * allEliminated
 * Returns 1 if every player has 0 HP (nobody is left alive), otherwise 0.
 */
int allEliminated(void)
{
    int p;

    for (p = 0; p < playerCount; p++)
    {
        if (players[p].health > 0)
        {
            return 0;   /* at least one player is still alive */
        }
    }
    return 1;
}

/*
 * isGameOver
 * The game ends when all treasures have been collected or all players have
 * been eliminated.
 */
int isGameOver(void)
{
    return (remainingTreasures() == 0) || allEliminated();
}

/*
 * isOccupied
 * Returns 1 if a player other than movingIndex is standing on (x, y).
 * Used to stop two players from sharing the same tile.
 */
int isOccupied(int x, int y, int movingIndex)
{
    int p;

    for (p = 0; p < playerCount; p++)
    {
        if (p != movingIndex && players[p].x == x && players[p].y == y)
        {
            return 1;
        }
    }
    return 0;
}

/*
 * movePlayer
 * Reads a move string for the given player, validates its length, then walks
 * through each move one step at a time. Each step is checked for walls, edges,
 * and locked doors (which consume a key). After a successful step the tile
 * effects are applied and the player symbol is redrawn at the new position.
 */
void movePlayer(int index)
{
    char input[100];
    int  len, i;

    printf("%s, enter moves (up to 4 of W/A/S/D): ", players[index].name);

    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        return;   /* no input available */
    }
    input[strcspn(input, "\n")] = '\0';   /* drop the trailing newline */

    len = strlen(input);
    if (len > 4)
    {
        printf("More than 4 moves entered - turn cancelled.\n");
        return;
    }

    for (i = 0; i < len; i++)
    {
        char c = toupper(input[i]);
        int  dx = 0, dy = 0;
        int  nx, ny;

        if      (c == 'W') dx = -1;   /* up    */
        else if (c == 'S') dx =  1;   /* down  */
        else if (c == 'A') dy = -1;   /* left  */
        else if (c == 'D') dy =  1;   /* right */
        else
        {
            printf("Invalid move '%c' skipped.\n", input[i]);
            continue;
        }

        nx = players[index].x + dx;
        ny = players[index].y + dy;

        /* Another player already occupies the target tile. */
        if (isOccupied(nx, ny, index))
        {
            printf("Another player is standing there.\n");
            continue;
        }

        /* Locked door: only passable if the player holds a key. */
        if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE &&
            map[nx][ny] == DOOR)
        {
            if (players[index].keys > 0)
            {
                players[index].keys--;
                map[nx][ny] = EMPTY;   /* door unlocked, now passable */
            }
            else
            {
                printf("A locked door blocks the way - you need a key.\n");
                continue;
            }
        }
        else if (!isValidMove(nx, ny))
        {
            printf("Move blocked by a wall or the edge.\n");
            continue;
        }

        /* Perform the step: leave the old tile, move, apply effects, redraw. */
        map[players[index].x][players[index].y] = EMPTY;
        players[index].x = nx;
        players[index].y = ny;
        processTile(index);
        map[nx][ny] = players[index].symbol;
    }
}

/*
 * printMap
 * Renders the visible grid to the console followed by the player HUD, which
 * shows each player's health, score, and key count.
 */
void printMap(void)
{
    int row, col, i;

    /* Draw the grid. */
    for (row = 0; row < GRID_SIZE; row++)
    {
        for (col = 0; col < GRID_SIZE; col++)
        {
            printf("%c ", map[row][col]);
        }
        printf("\n");
    }

    /* Draw the player HUD. */
    printf("\n");
    for (i = 0; i < playerCount; i++)
    {
        printf("[%c] %-10s  HP: %3d   Score: %3d   Keys: %d\n",
               players[i].symbol, players[i].name,
               players[i].health, players[i].score, players[i].keys);
    }
}

int main(void)
{
    printf("=================================\n");
    printf("   Quest for the Lost Treasure\n");
    printf("=================================\n");

    srand((unsigned int)time(NULL));   /* seed the random generator once */

    initializeMap();
    placeWalls();
    placeTreasures();
    placeHealthPacks();
    placeKeys();
    placeDoors();
    placeTraps();
    placePlayers();
    printMap();

    return 0;
}
