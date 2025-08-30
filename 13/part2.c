#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>

computer c = {0};
ringbuffer out, in;
#define FIELDSIZE 100
#define MAX(x, y) (x > y ? x : y)
int maxx = 0, maxy = 0;
char tiles[FIELDSIZE][FIELDSIZE];

#define MOVEUP(n) ("\x1B[" #n "A")
#define POS_SAVE "\x1B 7"
#define POS_REST "\x1B 8"

int read_direction() {
    int ret = 0;
    // Read input direction
    char dir = getch();
    int n = 0;
    switch (dir) {
        case 'a':
        case 'A':
            ret = -1;
            break;
        case 'd':
        case 'D':
            ret = 1;
            break;
        default:
            ret = 0;
    }
    return ret;
}

void clear_screen()
{
    printf("\x1B[2J");
}

void print_tiles(WINDOW* w)
{
    box(w, 0, 0);
    for (int r = 0; r <= maxy; r++)
    {
        wmove(w, r+1, 1);
        for (int c = 0; c <= maxx; c++)
        {
            wprintw(w, "%c", tiles[r][c]);
        }
    }
    wrefresh(w);
}

char symbols[] =
    {
        [0] = ' ',
        [1] = '#',
        [2] = 'x',
        [3] = '=',
        [4] = 'O',
    };

int main(int argc, char** argv) {
    // Initialize to 0
    reset_buffer(&out);
    reset_buffer(&in);
    c.out_buffer = &out;
    c.in_buffer = &in;

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

    // Insert quarter
    int64_t score = 0;
    c.memory[0] = 2;
    WINDOW *w = NULL;
    while (!c.halted)
    {
        process(&c);
        while(out.read_index < out.write_index)
        {
            int64_t x, y, v;
            ring_pop(&out, &x);
            ring_pop(&out, &y);
            ring_pop(&out, &v);
            if (x == -1) {
                score = v;
            }
            tiles[y][x] = symbols[v];

            maxx = MAX(x, maxx);
            maxy = MAX(y, maxy);
        }
        if (w == NULL) {
            initscr();
            cbreak();
            noecho();
            w = newwin(maxy+3, maxx+3, 0, 0);
            wrefresh(w);
        }
        print_tiles(w);
        int padx, ballx;
        for (int r = 0; r <= maxy; r++) {
            for (int c = 0; c <= maxx; c++) {
                if (tiles[r][c] == 'O') ballx = c;
                if (tiles[r][c] == '=') padx = c;
            }
        }
        int dir = padx < ballx ? 1 : padx > ballx ? -1 : 0;
        // Use this for iteractive play
        // int dir = read_direction();
        usleep(1000);
        ring_push(&in, dir);
    }
    delwin(w);
    nocbreak();
    echo();
    endwin();

    printf("computer halted %d \n", c.halted);
    printf("Score: %d\n", score);
    return 0;
}
