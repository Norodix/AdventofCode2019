#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>

computer c = {0};
ringbuffer out, in;
#define FIELDSIZE 50

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

    int sum = 0;
    int64_t val;
    for(int row = 0; row < FIELDSIZE; row++) {
        for(int col = 0; col < FIELDSIZE; col++) {
            ring_push(c.in_buffer, col);
            ring_push(c.in_buffer, row);
            process(&c);
            ring_pop(c.out_buffer, &val);
            printf("%c", val ? '#' : '.');
            sum += val;
            reset_memory(&c);
            reset_buffer(c.in_buffer);
            reset_buffer(c.out_buffer);
        }
        printf("\n");
    }
    printf("Sum: %d\n", sum);
    return 0;
}
