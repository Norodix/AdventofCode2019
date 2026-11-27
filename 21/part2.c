#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

computer c = {0};
ringbuffer out, in;

void test_springcode(char* springcode) {
    // Initialize to 0
    reset_memory(&c);
    reset_buffer(&out);
    reset_buffer(&in);
    c.out_buffer = &out;
    c.in_buffer = &in;
    for (int i = 0; i < strlen(springcode); i++) {
        ring_push(&in, springcode[i]);
    }

    process(&c);
    for (int i = 0; i < 1000000; i++)
    {
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
    reset_memory(&c);
    memdump(&c, memlen);

    // If A, B or C is hole and D is not, jump
    // If C and E are low its a trick, dont jump
    // Otherwise try to hit the far edge of every hole -> D and not C
    char* springcode =
                        "OR D J\n"
                        "AND H J\n" // D and H are ok, jump
                        "OR D T\n"
                        "AND E T\n"
                        "AND I T\n" // DEI are ok, jump
                        "OR T J\n" // Store jump
                        "OR A T\n" // This is not valid, but A=0 is handled separately anyway
                        "AND B T\n"
                        "AND C T\n" // A and B and C -> only floor, don't jump
                        "NOT T T\n" // T high -> there is a hole
                        "AND T J\n" // Gate jump on condition that there is a hole
                        "NOT A T\n"
                        "OR T J\n" // Jump always if right in front of hole
                        "RUN\n"
                        ;

    test_springcode(springcode);


    return 0;
}
