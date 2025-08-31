#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>

computer c = {0};
ringbuffer out, in;
#define FIELDSIZE 1000
#define CENTER (FIELDSIZE/2)
#define MAX(x, y) (x > y ? x : y)
int maxx = 0, maxy = 0;
char tiles[FIELDSIZE][FIELDSIZE];
WINDOW *w = NULL;

int dirstack[10000];
int stack_pointer = -1;

#define NORTH 1
#define SOUTH 2
#define WEST  3
#define EAST  4

#define EMPTY  0
#define WALKED 1
#define WALL   2
#define OXYGEN 3
#define TILECHARS (" .#O")

int read_direction() {
    int ret = 0;
    // Read input direction
    char dir = getch();
    int n = 0;
    switch (dir) {
        case 'w':
            ret = NORTH;
            break;
        case 's':
            ret = SOUTH;
            break;
        case 'a':
            ret = WEST;
            break;
        case 'd':
            ret = EAST;
            break;
        default:
            ret = 0;
    }
    return ret;
}

void print_tiles(WINDOW* w, int posx, int posy, int size)
{
    wresize(w, size*2+4, size*2+4);
    box(w, 0, 0);
    int line = 1;
    for (int r = posy-size; r <= posy+size; r++)
    {
        wmove(w, line, 1);
        for (int c = posx-size; c <= posx+size; c++)
        {
            if (r == posy && c == posx)
                wprintw(w, "x");
            else
                wprintw(w, "%c", TILECHARS[tiles[r][c]]);
        }
        line++;
    }
    wrefresh(w);
}

void print_center(WINDOW* w, int posx, int posy, int size)
{
    wresize(w, size*2+4, size*2+4);
    box(w, 0, 0);
    int line = 1;
    for (int r = CENTER-size; r <= CENTER+size; r++)
    {
        wmove(w, line, 1);
        for (int c = CENTER-size; c <= CENTER+size; c++)
        {
            if (r == posy && c == posx)
                wprintw(w, "%c", 'x');
            else
                wprintw(w, "%c", TILECHARS[tiles[r][c]]);
        }
        line++;
    }
    wrefresh(w);
}

int opposite_dir(int d)
{
    if (d == NORTH) return SOUTH;
    if (d == SOUTH) return NORTH;
    if (d == WEST) return EAST;
    if (d == EAST) return WEST;
    return 0;
}

char* get_tile_by_dir(int r, int c, int d)
{
    if (d == NORTH) r--;
    if (d == SOUTH) r++;
    if (d == WEST) c--;
    if (d == EAST) c++;
    return &tiles[r][c];
}

void set_tile_by_dir(int r, int c, int d, int v)
{
    if (d == NORTH) r--;
    if (d == SOUTH) r++;
    if (d == WEST) c--;
    if (d == EAST) c++;
    tiles[r][c] = v;
}

int test_move(computer* c, int dir)
{
    int ret = 0;
    ring_push(c->in_buffer, dir);
    process(c);
    int64_t response;
    ring_pop(c->out_buffer, &response);
    ret = response;
    if (response) {
        ring_push(c->in_buffer, opposite_dir(dir));
        process(c);
        int64_t response;
        ring_pop(c->out_buffer, &response);
    }
    return ret;
}

void map_neighbors(computer* c, int posx, int posy) {
    for (int i = 1; i <=4; i++) {
        int neighbor = 0;
        int64_t response;
        ring_push(c->in_buffer, i);
        process(c);
        ring_pop(c->out_buffer, &response);
        neighbor = response;
        if (response) {
            ring_push(c->in_buffer, opposite_dir(i));
            process(c);
            int64_t response;
            ring_pop(c->out_buffer, &response);
        }
        char* t = get_tile_by_dir(posy, posx, i);
        if (neighbor == 0) *t = WALL;
        if (neighbor == 2) *t = OXYGEN;
    }
}

void take_step(computer* c, int dir) {
        ring_push(c->in_buffer, dir);
        process(c);
        int64_t response;
        ring_pop(c->out_buffer, &response);
}

int found_goal = 0;
int goal_steps = 0;
void dfs(computer* c, int posx, int posy)
{
    if (tiles[posy][posx] == OXYGEN) found_goal = 1;
    // usleep(10000);
    map_neighbors(c, posx, posy);
    print_center(w, posx, posy, 25);
    for (int i = 1; i <= 4 && found_goal == 0; i++)
    {
        char* dirtile = get_tile_by_dir(posy, posx, i);
        if (*dirtile == EMPTY || *dirtile == OXYGEN)
        {
            if (*dirtile == EMPTY) *dirtile = WALKED;
            int newy = posy;
            int newx = posx;
            switch(i)
            {
                case NORTH:
                    newy--;
                    break;
                case SOUTH:
                    newy++;
                    break;
                case WEST:
                    newx--;
                    break;
                case EAST:
                    newx++;
                    break;
            }
            take_step(c, i);
            dfs(c, newx, newy);
            goal_steps += found_goal;
            take_step(c, opposite_dir(i));
        }
    }
}


int main(int argc, char** argv) {
    // Initialize to 0
    reset_buffer(&out);
    reset_buffer(&in);
    c.out_buffer = &out;
    c.in_buffer = &in;
    int posx = CENTER, posy = CENTER;

    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }
    char line[0x8000];
    fgets(line, sizeof(line), f);

    int memlen = 0;
    while (!feof(f)) {
        memlen = parse_memory(line);
        fgets(line, sizeof(line), f);
    }
    reset_memory(&c);
    memdump(&c, memlen);
    int blocks = 0;

    memset(tiles, 0, FIELDSIZE * FIELDSIZE * sizeof(tiles[0][0]));
    initscr();
    cbreak();
    noecho();
    refresh();
    w = newwin(40, 40, 0, 0);
    dfs(&c, posx, posy);
    print_center(w, -1000, -1000, 25);
    getch();

    // clear up terminal
    delwin(w);
    nocbreak();
    echo();
    endwin();

    printf("Found goal and it took %d steps\n", goal_steps);

    return 0;
}
