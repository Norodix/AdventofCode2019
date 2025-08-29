#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

computer c = {0};
ringbuffer out;
#define FIELDSIZE 100
#define MAX(x, y) (x > y ? x : y)
char tiles[FIELDSIZE][FIELDSIZE];

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
    c.out_buffer = &out;

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
    int maxx = 0, maxy = 0;

    while(out.read_index < out.write_index)
    {
        int64_t x, y, v;
        ring_pop(&out, &x);
        ring_pop(&out, &y);
        ring_pop(&out, &v);
        printf("(%d, %d) -> %c\n", x, y, symbols[v]);
        tiles[y][x] = symbols[v];

        maxx = MAX(x, maxx);
        maxy = MAX(y, maxy);
    }

    int blocks = 0;

    for (int r = 0; r <= maxy; r++)
    {
        for (int c = 0; c <= maxx; c++)
        {
            printf("%c", tiles[r][c]);
            if (tiles[r][c] == 'x') blocks++;
        }
        printf("\n");
    }

    printf("Total number of blocks: %d\n", blocks);

    return 0;
}
