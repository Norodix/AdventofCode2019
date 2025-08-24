#include <stdint.h>

#define MEMORY_MAX (0x1000)
extern int64_t memory[MEMORY_MAX];

int parse_memory(char* str);
void process();
void disas_prog(int len);
void disas_inst(int addr);
void memdump(int len);
void reset_memory();
