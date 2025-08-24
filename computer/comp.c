#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <comp.h>

#define LINELEN (0x4000) // maximum length of lines to be read

#define INST_INVALID     0
#define INST_HALT       99
#define INST_ADD         1
#define INST_MUL         2
#define INST_INPUT       3
#define INST_OUTPUT      4
#define INST_JUMP_TRUE   5
#define INST_JUMP_FALSE  6
#define INST_LESS        7
#define INST_EQUAL       8

int64_t memory[MEMORY_MAX];
static int64_t memory_initial[MEMORY_MAX];
static uint8_t halted = 0;
static int pc = 0;

void memdump(int len)
{
    for (int row = 0; row < len/10; row++){
        printf("%3d: ", row*10);
        for(int i = 0; i + row*10 < len && i < 10; i++){
            printf("%6ld\t", memory[i+row*10]);
        }
        printf("\n");
    }
    printf("\n");
}

void reset_memory() {
    memcpy(memory, memory_initial, sizeof(memory));
}

// Is the nth arg immediate?
static int is_imm(int64_t inst, int n)
{
    inst /= 100;
    for (int i = 1; i < n; i++) {
        inst /= 10;
    }
    // Immediate mode
    return inst & 1;
}

static int64_t get_arg(int64_t *m, int n) {
    // Immediate mode
    if (is_imm(*m, n)) {
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

static void jump_true(int64_t *m)
{
    int64_t arg1 = get_arg(m, 1);
    if (arg1 != 0) {
        pc = get_arg(m, 2) - 3;
    }
}

static void jump_false(int64_t *m)
{
    int64_t arg1 = get_arg(m, 1);
    if (arg1 == 0) {
        pc = get_arg(m, 2) - 3;
    }
}

static void less_than(int64_t *m)
{
    int val = 0;
    if (get_arg(m, 1) < get_arg(m, 2))
    {
        val = 1;
    }
    memory[*(m+3)] = val;
}

static void equal(int64_t *m)
{
    int val = 0;
    if (get_arg(m, 1) == get_arg(m, 2))
    {
        val = 1;
    }
    memory[*(m+3)] = val;
}

struct Op {
    char name[4];
    int argcount;
    void (*operation)(int64_t*);
};

static struct Op operation[] = {
    [INST_INVALID] = {
        .name = "???",
        .argcount = 0,
        .operation = NULL,
    },
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
    [INST_JUMP_TRUE] = {
        .name = "jpt",
        .argcount = 2,
        .operation = jump_true,
    },
    [INST_JUMP_FALSE] = {
        .name = "jpf",
        .argcount = 2,
        .operation = jump_false,
    },
    [INST_LESS] = {
        .name = "lss",
        .argcount = 3,
        .operation = less_than,
    },
    [INST_EQUAL] = {
        .name = "equ",
        .argcount = 3,
        .operation = equal,
    },
    [INST_HALT] = {
        .name = "hlt",
        .argcount = 0,
        .operation = halt,
    }
};

static struct Op get_op(int opindex) {
    int index = opindex%100;
    if (operation[index].name[0] == 0) return operation[INST_INVALID];
    return operation[opindex%100];
}

static struct Op get_op_by_addr(int addr) {
    return get_op(memory[addr]);
}

void disas_inst(int addr)
{
    struct Op instruction = get_op_by_addr(addr);
    printf("%04d: %4ld %s", addr, memory[addr], instruction.name);
    for (int i = 0; i < instruction.argcount; i++)
    {
        if (is_imm(memory[addr], i+1)) {
            printf("\t#%ld", memory[addr+i+1]);
        }
        else {
            printf("\t%ld->#%ld", memory[addr+i+1], memory[memory[addr+i+1]]);
        }
    }
    printf("\n");
}

void disas_prog(int len)
{
    int pc = 0;
    struct Op instruction = get_op_by_addr(pc);
    while(pc < len)
    {
        disas_inst(pc);
        pc += instruction.argcount + 1;
        instruction = get_op_by_addr(pc);
    }
    fflush(stdout);
}


void process()
{
    pc = 0;
    halted = 0;

    struct Op instruction = get_op_by_addr(pc);
    while(!halted)
    {
        disas_inst(pc);
        if (instruction.operation) instruction.operation(&memory[pc]);
        pc += instruction.argcount + 1;
        instruction = get_op_by_addr(pc);
    }
}

// TODO this can cut off a number in the middle. Fix that
// This function can be called multiple times
// it remembers where it left off
int parse_memory(char* str) {
    static int index = 0;
    if (index == 0) memset(memory_initial, 0, sizeof(memory));
    printf("Parsing numbers\n");

    char* token = strtok(str, ",\n");
    while(token != NULL)
    {
        memory_initial[index] = (int64_t)strtol(token, NULL, 10);
        index++;
        token = strtok(NULL, ",\n");
    }
    return index;
}

