#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>

computer c = {0};
ringbuffer out, in;
#define FIELDSIZE 90

int64_t get_tile(int row, int col) {
    int64_t retval;
    ring_push(c.in_buffer, col);
    ring_push(c.in_buffer, row);
    process(&c);
    ring_pop(c.out_buffer, &retval);
    reset_memory(&c);
    reset_buffer(c.in_buffer);
    reset_buffer(c.out_buffer);
    return retval;
}

// return 1 if at edge like # <- here
//                          .
int is_edge(int row, int col) {
    if (get_tile(row, col) && !get_tile(row+1, col))
        return 1;
    return 0;
}

// row column is the top left coords
int check_square(int row, int col, int size) {
    int is_full = 1;

    for(int r = 0; r < size; r++) {
        for(int c = 0; c < size; c++) {
            if (!get_tile(row+r, col+c)) {
                is_full = 0;
            }
            else {
            }
        }
    }
    return is_full;
}

void print_area(int row, int col, int size) {
    for(int r = 0; r < size; r++) {
        for(int c = 0; c < size; c++) {
            if (!get_tile(row+r, col+c)) {
                printf("%c", ' ');
            }
            else {
                printf("%c", '#');
            }
        }
        printf("\n");
    }
}

int check_across(int row, int col, int size) {
    for (int i = 0; i < size; i++) {
        if (!get_tile(row-i, col+i))
        {
            return 0;
        }
    }
    return 1;
}

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
    int row = 0, col = 99;
    int found = 0;
    while (!found) {
        col++;
        // walk down to the edge
        while (!is_edge(row, col)) row++;
        int is_100_across = check_across(row, col, 100);
        if (is_100_across) break;
    }
    row -= 99;
    printf("Top left coordinates (r,c) where top right is 100 at least: (%d,%d)\n", row, col);
    printf("(%d, %d) is full: %d\n", row,   col,   check_square(row,   col,   100));
    printf("(%d, %d) is full: %d\n", row-1, col-1, check_square(row-1, col-1, 100));
    printf("(%d, %d) is full: %d\n", row-1, col,   check_square(row-1, col,   100));
    printf("(%d, %d) is full: %d\n", row,   col-1, check_square(row,   col-1, 100));
    printf("Calculated value: %d\n", col * 10000 + row);
    // 10730410 is too low -> off by 1 :')
    return 0;
}
