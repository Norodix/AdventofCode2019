#include <stdint.h>

#define MEMORY_MAX (0x1000)
extern int64_t memory[MEMORY_MAX];

typedef struct computer {
    int64_t memory[MEMORY_MAX];
    uint8_t halted;
    int pc;
}computer;

/**
 * @brief Parse the input csv string to initial memory buffer
 *
 * @param str input csv
 * @return Returns the number of memory addresses written
 */
int parse_memory(char* str);

void process(computer*);
void disas_prog(computer*, int len);
void disas_inst(computer*, int);
void memdump(computer*, int len);
void reset_memory(computer*);
