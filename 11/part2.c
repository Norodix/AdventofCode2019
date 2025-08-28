#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

computer c = {0};
ringbuffer in;
ringbuffer out;

#define SURFACE_SIZE    2000
#define CENTER          (SURFACE_SIZE/2)

#define BLACK 0
#define WHITE 1
#define COLOR_MASK 0x7F
#define TOUCH_MASK 0x80

enum dir {
    DIR_UP = 0,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
    DIR_MAX,
};

static char tiles[SURFACE_SIZE][SURFACE_SIZE];
void print_center(int size)
{
    for (int row = CENTER-size; row < CENTER + size; row++)
    {
        for (int col = CENTER-size; col < CENTER + size; col++)
        {
            char c = (tiles[row][col] & COLOR_MASK) ? '#' : '.';
            if ((tiles[row][col] & TOUCH_MASK) == 0) c = ' ';
            printf("%c", c);
        }
        printf("\n");
    }
}



int main(int argc, char** argv) {
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

    memset(tiles, BLACK, sizeof(tiles));
    tiles[CENTER][CENTER] = WHITE;

    int64_t robot_dir = DIR_UP;
    int64_t robot_row = CENTER;
    int64_t robot_col = CENTER;

    reset_buffer(&in);
    reset_buffer(&out);
    c.in_buffer = &in;
    c.out_buffer = &out;
    reset_memory(&c);
    memdump(&c, memlen);
    while (!c.halted)
    {
        ring_push(&in, tiles[robot_row][robot_col] & COLOR_MASK);
        process(&c);
        if (c.halted) break;
        int64_t color;
        ring_pop(&out, &color);
        tiles[robot_row][robot_col] = color;
        tiles[robot_row][robot_col] |= TOUCH_MASK;
        int64_t d_dir;
        ring_pop(&out, &d_dir);
        robot_dir--;
        robot_dir += d_dir*2;
        robot_dir += DIR_MAX;
        robot_dir %= DIR_MAX;
        switch (robot_dir) {
            case DIR_UP:
                robot_row--;
                break;
            case DIR_DOWN:
                robot_row++;
                break;
            case DIR_LEFT:
                robot_col--;
                break;
            case DIR_RIGHT:
                robot_col++;
                break;
        }
        printf("Direction: %d\n", robot_dir);
        // print_center(10);
    }
    print_center(50);

    // Count the tiles that have been patined
    int tiles_touched = 0;
    for (int row = 0; row < SURFACE_SIZE; row++)
    {
        for (int col = 0; col < SURFACE_SIZE; col++)
        {
            tiles_touched += (tiles[row][col] & TOUCH_MASK) ? 1 : 0;
        }
    }

    printf("Tiles that will be painted at least once: %d\n", tiles_touched);
    return 0;
}
