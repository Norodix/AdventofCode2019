#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char** argv) {
    // Initialize to 0

    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }
    char line[1024];
    fgets(line, sizeof(line), f);

    int memlen = 0;
    while (!feof(f)) {
        memlen = parse_memory(line);
        reset_memory();
        memdump(memlen);
        fgets(line, sizeof(line), f);
    }
    reset_memory();
    memdump(memlen);
    disas_prog(memlen);
    process();
    return 0;
}
