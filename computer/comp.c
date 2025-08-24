#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <comp.h>

#define LINELEN (0x4000) // maximum length of lines to be read

#define INST_HALT   99
#define INST_ADD     1
#define INST_MUL     2
#define INST_INPUT   3
#define INST_OUTPUT  4

int64_t memory[MEMORY_MAX];
static int64_t memory_initial[MEMORY_MAX];
static uint8_t halted = 0;

void memdump(int len)
{
    for(int i = 0; i < len; i++){
        printf("%lu,", memory[i]);
    }
    printf("\n");
}

void reset_memory() {
    memcpy(memory, memory_initial, sizeof(memory));
}

static int64_t get_arg(int64_t *m, int n) {
    int64_t inst = *m / 100;
    for (int i = 1; i < n; i++) {
        inst /= 10;
    }
    // Immediate mode
    if (inst & 1) {
        return *(m+n);
    }
    else {
        return memory[*(m+n)];
    }
}

static void add(int64_t *m) {
    memory[*(m+3)] = get_arg(m, 1) + get_arg(m, 2);
}

static void mult(int64_t *m) {
    memory[*(m+3)] = get_arg(m, 1) * get_arg(m, 2);
}

static void input(int64_t *m) {
    char str[100] = "\0";
    printf("\n>");
    fgets(str, 100, stdin);
    memory[*(m+1)] = strtol(str, NULL, 10);
}

static void output(int64_t *m) {
    printf("%ld\n", get_arg(m, 1));
}

static void halt(int64_t *m)
{
    halted = 1;
}

struct Op {
    char name[4];
    int argcount;
    void (*operation)(int64_t*);
};

// static void (*operation[0xFF])(uint64_t, uint64_t, uint64_t) = {
static struct Op operation[] = {
    [INST_ADD] = {
        .name = "add",
        .argcount = 3,
        .operation = add,
    },
    [INST_MUL] = {
        .name = "mul",
        .argcount = 3,
        .operation = mult,
    },
    [INST_INPUT] = {
        .name = "inp",
        .argcount = 1,
        .operation = input,
    },
    [INST_OUTPUT] = {
        .name = "out",
        .argcount = 1,
        .operation = output,
    },
    [INST_HALT] = {
        .name = "hlt",
        .argcount = 0,
        .operation = halt,
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
    halted = 0;

    struct Op instruction = operation[memory[pc]];
    while(!halted)
    {
        instruction.operation(&memory[pc]);
        pc += instruction.argcount + 1;
        instruction = operation[memory[pc]];
    }
}

// This function can be called multiple times
// it remembers where it left off
int parse_memory(char* str) {
    static int index = 0;
    if (index == 0) memset(memory_initial, 0, sizeof(memory));

    char* token = strtok(str, ",\n");
    while(token != NULL)
    {
        memory_initial[index] = (uint8_t)strtoul(token, NULL, 10);
        index++;
        token = strtok(NULL, ",\n");
    }
    return index;
}

