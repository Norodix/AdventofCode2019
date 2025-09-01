#include <stdint.h>

#define MEMORY_MAX (0x10000)
#define IO_BUFFER_LEN 10000

typedef struct ringbuffer {
    int64_t buffer[IO_BUFFER_LEN];
    int write_index;
    int read_index;
}ringbuffer;

typedef struct computer {
    int64_t *memory;
    int64_t memsize; // in bytes
    uint8_t halted;
    int pc;
    int blocked;
    int relative_base;
    struct ringbuffer *in_buffer;
    struct ringbuffer *out_buffer;
}computer;

/**
 * @brief Parse the input csv string to initial memory buffer
 *
 * @param str input csv
 * @return Returns the number of memory addresses written
 */
int parse_memory(char* str);
void init_computer(computer *c);
int ring_push(ringbuffer *r, int64_t v);
int64_t ring_pop(ringbuffer *r, int64_t *v);
void dump_buffer(ringbuffer *r);
void process(computer*);
void disas_prog(computer*, int len);
void disas_inst(computer*, int);
void memdump(computer*, int len);
void reset_memory(computer*);
void reset_buffer(ringbuffer* r);
