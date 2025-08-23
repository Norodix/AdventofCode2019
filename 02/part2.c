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
static uint64_t memory_initial[MEMORY_MAX];
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

struct Op {
    char name[4];
    int argcount;
    void (*operation)();
};

// static void (*operation[0xFF])(uint64_t, uint64_t, uint64_t) = {
struct Op operation[] = {
    [ADD] = {
        .name = "add",
        .argcount = 3,
        .operation = add,
    },
    [MUL] = {
        .name = "mul",
        .argcount = 3,
        .operation = mult,
    },
    [HALT] = {
        .name = "hlt",
        .argcount = 0,
        .operation = NULL,
    }
};

void disas_inst(int addr)
{
    struct Op instruction = operation[memory[addr]];
    printf("%s", instruction.name);
    for (int i = 0; i < instruction.argcount; i++)
    {
        printf("\t%lu", memory[addr+i+1]);
    }
    printf("\n");
}

void disas_prog(int len)
{
    int pc = 0;
    struct Op instruction = operation[memory[pc]];
    while(pc < len)
    {
        disas_inst(pc);
        pc += instruction.argcount + 1;
        instruction = operation[memory[pc]];
    }
    fflush(stdout);
}


void process()
{
    int pc = 0;
    struct Op instruction = operation[memory[pc]];
    while(memory[pc] != HALT)
    {
        uint64_t a = memory[pc+1];
        uint64_t b = memory[pc+2];
        uint64_t t = memory[pc+3];
        fflush(stdout);

        instruction.operation(a, b, t);
        pc += instruction.argcount + 1;
        instruction = operation[memory[pc]];
    }
}


int main(int argc, char** argv) {
    // Initialize to 0
    memset(memory_initial, 0, sizeof(memory));

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
            memory_initial[index] = (uint8_t)strtoul(token, NULL, 10);
            index++;
            token = strtok(NULL, ",\n");
        }
        fgets(line, sizeof(line), f);
    }

    memcpy(memory, memory_initial, sizeof(memory));
    disas_prog(index);

    for (int noun = 0; noun < 100; noun++)
    {
        for (int verb = 0; verb < 100; verb++)
        {
            memcpy(memory, memory_initial, sizeof(memory));
            memory[1] = noun;
            memory[2] = verb;
            process();
            if (memory[0] == 19690720)
            {
                printf("Found target at verb %d and noun %d\n", verb, noun);
                printf("Solution: %d\n", 100 * noun + verb);
                return 0;
            }
        }
    }

    return -1;
}
