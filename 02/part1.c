#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LINELEN (0x4000) // maximum length of lines to be read

#define MEMORY_MAX (0x1000)
#define HALT 99
#define ADD 1
#define MUL 2

static uint64_t memory[MEMORY_MAX];
static uint8_t halted = 0;

void memdump(int len)
{
    for(int i = 0; i < len; i++){
        printf("%lu,", memory[i]);
    }
    printf("\n");
}

void add(uint64_t a, uint64_t b, uint64_t target) {
    memory[target] = memory[a] + memory[b];
}

void mult(uint64_t a, uint64_t b, uint64_t target) {
    memory[target] = memory[a] * memory[b];
}

static void (*operation[0xFF])(uint64_t, uint64_t, uint64_t) = {
[ADD] = add,
[MUL] = mult,
};

void process()
{
    int pc = 0;
    int instruction = memory[pc];
    while(memory[pc] != HALT)
    {
        uint64_t a = memory[pc+1];
        uint64_t b = memory[pc+2];
        uint64_t t = memory[pc+3];
        printf("%lu %lu %lu\n", a, b, t);

        operation[instruction](a, b, t);
        pc += 4;
        instruction = memory[pc];
    }
}


int main(int argc, char** argv) {
    // Initialize to 0
    memset(memory, 0, sizeof(memory));

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

    // Parse the input into memory
    int index = 0;
    while (!feof(f)) {
        // HERE something is fucked up. a 0 instruction is inserted between the lines
        char* token = strtok(line, ",\n");
        while(token != NULL)
        {
            memory[index] = (uint8_t)strtoul(token, NULL, 10);
            // printf("%d,", memory[index]);
            index++;
            token = strtok(NULL, ",\n");
        }
        fgets(line, sizeof(line), f);
    }

    memdump(index);
    process();
    memdump(index);


    return 0;
}
