#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <comp.h>

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
#define INST_BASE_ADJ    9

enum arg_mode{
    MODE_POSITION = 0,
    MODE_DIRECT,
    MODE_RELATIVE,
};

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
    c->relative_base = 0;
    memcpy(c->memory, memory_initial, sizeof(c->memory));
}

void reset_buffer(ringbuffer* r)
{
    memset(r, 0, sizeof(ringbuffer));
}

static int get_arg_mode(int64_t inst, int n)
{
    inst /= 100;
    for (int i = 1; i < n; i++) {
        inst /= 10;
    }
    inst %= 10;
    return inst;
}

static int get_addr(computer* c, int64_t *m, int n) {
    int addr = 0;
    switch (get_arg_mode(*m, n)){
        case MODE_POSITION:
            addr = *(m+n);
            break;
        case MODE_RELATIVE:
            addr = *(m+n) + c->relative_base;
            break;
    }
    return addr;
}

static int64_t get_arg(computer* c, int64_t *m, int n) {
    int64_t argument = 0;
    switch (get_arg_mode(*m, n)){
        case MODE_POSITION:
            argument = c->memory[*(m+n)];
            break;
        case MODE_DIRECT:
            argument = *(m+n);
            break;
        case MODE_RELATIVE:
            argument = c->memory[*(m+n) + c->relative_base];
            break;
    }
    return argument;
}

static void add(computer* c, int64_t *m) {
    int addr = get_addr(c, m, 3);
    c->memory[addr] = get_arg(c, m, 1) + get_arg(c, m, 2);
}

static void mult(computer* c, int64_t *m) {
    int addr = get_addr(c, m, 3);
    c->memory[addr] = get_arg(c, m, 1) * get_arg(c, m, 2);
}

static void base_adjust(computer* c, int64_t *m) {
    c->relative_base += get_arg(c, m, 1);
    // printf("base adjusted to %d\n", c->relative_base);
}

static void input(computer* c, int64_t *m) {
    int64_t num = 0;
    int success = 0;
    if (c->in_buffer == NULL)
    {
        char str[100] = "\0";
        printf("\n>");
        fgets(str, 100, stdin);
        num = strtol(str, NULL, 10);
    }
    else {
        success = ring_pop(c->in_buffer, &num);
        if (!success)
        {
            c->blocked = 1;
            return;
        }
    }
    int addr = get_addr(c, m, 1);
    c->memory[addr] = num;
}

static void output(computer* c, int64_t *m) {
    int64_t num = get_arg(c, m, 1);
    if (c->out_buffer == NULL) {
        printf("%ld\n", num);
    }
    else {
        ring_push(c->out_buffer, num);
    }
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
    int addr = get_addr(c, m, 3);
    c->memory[addr] = val;
}

static void equal(computer* c, int64_t *m)
{
    int val = 0;
    if (get_arg(c, m, 1) == get_arg(c, m, 2))
    {
        val = 1;
    }
    int addr = get_addr(c, m, 3);
    c->memory[addr] = val;
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
    [INST_BASE_ADJ] = {
        .name = "bsa",
        .argcount = 1,
        .operation = base_adjust,
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
        enum arg_mode mode = get_arg_mode(addr, i+1);
        int64_t arg = get_arg(c, c->memory+addr, i+1);
        switch (mode) {
            case MODE_DIRECT:
                printf("\t#%ld", arg);
                break;
            case MODE_POSITION:
                printf("\t%ld->#%ld", c->memory[addr+i+1], arg);
                break;
            case MODE_RELATIVE:
                printf("\t%ld+%d->#%ld", c->memory[addr+i+1], c->relative_base, arg);
                break;
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
        disas_inst(c, c->pc);
        #endif
        if (instruction.operation) instruction.operation(c, &c->memory[c->pc]);
        // If blocked, do not advance the counter but just exit the process, we will pick itup here
        if (c->blocked)
        {
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
    if (index == 0) memset(memory_initial, 0, sizeof(memory_initial));
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

