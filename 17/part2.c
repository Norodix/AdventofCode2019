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

void push_string(ringbuffer *r, char* s) {
    while (*s != 0) {
        ring_push(r, *s);
        s++;
    }
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

    process(&c);
    int row = 0, col = 0;
    printf("\n");
    printf("%2d ", row);
    while(out.write_index > out.read_index) {
        char c;
        ring_pop(&out, &c);
        if (c == '\n') {
            maxy = MAX(col, maxy);
            row++;
            col = 0;
        }
        else {
            maxx = MAX(col, maxx);
            tiles[row][col++] = c;
        }
        putchar(c);
        if (c == '\n')
            printf("%2d ", row);
    }

    int sum = 0;
    for (int r = 1; r < maxy-1; r++) {
        for (int c = 1; c < maxy-1; c++) {
            int intersection = \
                tiles[r][c] == '#' &&
                tiles[r+1][c] == '#' &&
                tiles[r-1][c] == '#' &&
                tiles[r][c+1] == '#' &&
                tiles[r][c-1] == '#';
            if (intersection) {
                sum += r*c;
            }
        }
    }
    printf("Total sum is %d\n", sum);

    // A,B,A,B,A,C,B,C,A,C
    // l10,l12,r6
    // r10,l4,l4,l12
    // l10,r10,r6,l4

    char main[]     = "A,B,A,B,A,C,B,C,A,C\x0A";
    char A[]        = "L,10,L,12,R,6\x0A";
    char B[]        = "R,10,L,4,L,4,L,12\x0A";
    char C[]        = "L,10,R,10,R,6,L,4\x0A";
    char video[] = "y\x0A";

    reset_memory(&c);
    reset_buffer(&in);
    reset_buffer(&out);

    push_string(&in, main);
    push_string(&in, A);
    push_string(&in, B);
    push_string(&in, C);
    push_string(&in, video);

    c.memory[0] = 2;

    while (!c.halted) {
        process(&c);
        while(out.write_index > out.read_index) {
            int64_t v;
            ring_pop(&out, &v);
            if (v > 0xFF) {
                printf("Got big number: %ld\n", v);
                continue;
            }
            putchar((char)v);
        }
    }

    return 0;
}
