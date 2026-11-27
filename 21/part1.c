#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

computer c = {0};
ringbuffer out, in;

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

// No matter what, if there is a hole in front, just jump
// If D is hole never jump
// Otherwise:
//@#..# -> yes
//@#.## -> yes
//@##.# -> yes
//@#### -> no

// Only jump in these situations
// #..#
// #.##
// ##.# -> no

// Otherwise try to hit the far edge of every hole -> D and not C
    char* springcode = "NOT A J\n" // There is a hole directly in front, just jump
                       "OR B T\n" // copy B to T
                       "AND C T\n" // B and C
                       "NOT T T\n" // Not B and C
                       "AND D T\n" // If so fare jump (T) and D is ground then jump
                       "OR T J\n"
                       "WALK\n"
                       ;

    for (int i = 0; i < strlen(springcode); i++) {
        ring_push(&in, springcode[i]);
    }

    for (int i = 0; i < 1000000; i++)
    {
        process(&c);
        int64_t x;
        if(ring_pop(&out, &x)) {
            if (x > 'z') {
                printf("Made it across!\n");
                printf("%ld\n", x);
                break;
            }
            else printf("%c", x);
        }
    }

    return 0;
}
