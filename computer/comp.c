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

static int64_t memory_initial[MEMORY_MAX];

void ring_push(ringbuffer *r, int64_t v){
    // lets assume we never overwrite it... we will worry about it later
    r->buffer[r->write_index] = v;
    r->write_index++;
}

int64_t ring_pop(ringbuffer *r, int64_t *v)
{
    if (r->read_index >= r->write_index)
    {
        return 0;
    }
    else
    {
        *v = r->buffer[r->read_index];
        r->read_index++;
        if (r->read_index == r->write_index) {
            r->read_index = 0;
            r->write_index = 0;
        }
        return 1;
    }
}

void dump_buffer(ringbuffer *r) {
    printf("buffer %d - %d\n{", r->read_index, r->write_index);
    for(int j = r->read_index; j < r->write_index; j++) {
        printf("%ld ", r->buffer[j]);
    }
    printf("}\n");
}

void memdump(computer* c, int len)
{
    for (int row = 0; row < len/10; row++){
        printf("%3d: ", row*10);
        for(int i = 0; i + row*10 < len && i < 10; i++){
            printf("%6ld\t", c->memory[i+row*10]);
        }
        printf("\n");
    }
    printf("\n");
}

void reset_memory(computer* c) {
    c->pc = 0;
    c->halted = 0;
    c->blocked = 0;
    memcpy(c->memory, memory_initial, sizeof(c->memory));
}

void reset_buffer(ringbuffer* r)
{
    memset(r, 0, sizeof(ringbuffer));
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

static int64_t get_arg(computer* c, int64_t *m, int n) {
    // Immediate mode
    if (is_imm(*m, n)) {
        return *(m+n);
    }
    else {
        return c->memory[*(m+n)];
    }
}

static void add(computer* c, int64_t *m) {
    c->memory[*(m+3)] = get_arg(c, m, 1) + get_arg(c, m, 2);
}

static void mult(computer* c, int64_t *m) {
    c->memory[*(m+3)] = get_arg(c, m, 1) * get_arg(c, m, 2);
}

void input(computer* c, int64_t *m) {
    int64_t num = 0;
    int success = 0;
    success = ring_pop(c->in_buffer, &num);
    if (!success)
    {
        c->blocked = 1;
        return;
    }
    c->memory[*(m+1)] = num;
}

void output(computer* c, int64_t *m) {
    int64_t num = get_arg(c, m, 1);
    ring_push(c->out_buffer, num);
}

static void halt(computer* c, int64_t *m)
{
    c->halted = 1;
}

static void jump_true(computer* c, int64_t *m)
{
    int64_t arg1 = get_arg(c, m, 1);
    if (arg1 != 0) {
        c->pc = get_arg(c, m, 2) - 3;
    }
}

static void jump_false(computer* c, int64_t *m)
{
    int64_t arg1 = get_arg(c, m, 1);
    if (arg1 == 0) {
        c->pc = get_arg(c, m, 2) - 3;
    }
}

static void less_than(computer* c, int64_t *m)
{
    int val = 0;
    if (get_arg(c, m, 1) < get_arg(c, m, 2))
    {
        val = 1;
    }
    c->memory[*(m+3)] = val;
}

static void equal(computer* c, int64_t *m)
{
    int val = 0;
    if (get_arg(c, m, 1) == get_arg(c, m, 2))
    {
        val = 1;
    }
    c->memory[*(m+3)] = val;
}

struct Op {
    char name[4];
    int argcount;
    void (*operation)(computer*, int64_t*);
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

static struct Op get_op_by_addr(computer* c, int addr) {
    return get_op(c->memory[addr]);
}

void disas_inst(computer* c, int addr)
{
    struct Op instruction = get_op_by_addr(c, addr);
    printf("%04d: %4ld %s", addr, c->memory[addr], instruction.name);
    for (int i = 0; i < instruction.argcount; i++)
    {
        if (is_imm(c->memory[addr], i+1)) {
            printf("\t#%ld", c->memory[addr+i+1]);
        }
        else {
            printf("\t%ld->#%ld", c->memory[addr+i+1], c->memory[c->memory[addr+i+1]]);
        }
    }
    printf("\n");
}

void process(computer* c)
{
    c->halted = 0;
    c->blocked = 0;

    struct Op instruction = get_op_by_addr(c, c->pc);
    while(!c->halted)
    {
        #ifdef DEBUG_INST
        disas_inst(c, pc);
        #endif
        if (instruction.operation) instruction.operation(c, &c->memory[c->pc]);
        // If blocked, do not advance the counter but just exit the process, we will pick itup here
        if (c->blocked)
        {
            // printf("Blocking\n");
            return;
        }
        c->pc += instruction.argcount + 1;
        instruction = get_op_by_addr(c, c->pc);
    }
}

// TODO this can cut off a number in the middle. Fix that
// This function can be called multiple times
// it remembers where it left off
int parse_memory(char* str) {
    static int index = 0;
    if (index == 0) memset(memory_initial, 0, sizeof(memory));
    // printf("Parsing numbers\n");

    char* token = strtok(str, ",\n");
    while(token != NULL)
    {
        memory_initial[index] = (int64_t)strtol(token, NULL, 10);
        index++;
        token = strtok(NULL, ",\n");
    }
    return index;
}

