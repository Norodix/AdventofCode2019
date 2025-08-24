#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// special case, it can be only 1 computer and 1 ringbuffer that is its own in and output
static computer c[5];
static ringbuffer r[5];

#define DEBUG 0


int64_t test_permutation(int64_t *phase_seq)
{
    int64_t res = 0;
    for (int i = 0; i < 5; i++)
    {
        reset_memory(&c[i]);
        reset_buffer(&r[i]);
    }
    for (int i = 0; i < 5; i++)
    {
        c[i].in_buffer = &r[i];
        c[i].out_buffer = &r[(i+1)%5];
        // provide phase setting as first instruction for each computer
        ring_push(c[i].in_buffer, phase_seq[i]);
    }
    ring_push(c[0].in_buffer, 0); // seed signal

#if DEBUG
    for (int i = 0; i < 5; i++)
    {
        dump_buffer(c[i].in_buffer);
    }
#endif

    int comp_index = 0;
    while (c[4].halted != 1){
        computer *comp = &c[comp_index];
        if (comp->halted) {
            printf("Should not happen, halted computer got its turn\n");
            continue;
        }
#if DEBUG
        printf("Input buffer of computer %d\n", comp_index);
        dump_buffer(comp->in_buffer);
#endif
        process(comp);
#if DEBUG
        printf("output buffer of computer %d\n", comp_index);
        dump_buffer(comp->out_buffer);
#endif
        comp_index++;
        comp_index %= 5;
    }
    // At last, last computer should have output a value
    ring_pop(c[4].out_buffer, &res);
    return res;
}

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
    char line[0x8000];
    fgets(line, sizeof(line), f);

    int memlen = 0;
    while (!feof(f)) {
        memlen = parse_memory(line);
        fgets(line, sizeof(line), f);
    }

    int64_t max = 0;
    int64_t perm[5];
    // worst way to generate permutations
    for (int i = 0; i < 5*5*5*5*5; i++)
    {
        perm[0] = i % 5;
        perm[1] = (i / (5)) % 5;
        perm[2] = (i / (5 * 5)) % 5;
        perm[3] = (i / (5 * 5 * 5)) % 5;
        perm[4] = (i / (5 * 5 * 5 * 5)) % 5;
        int sign = 0;
        for (int j = 0; j < 5; j++) sign |= 1 << perm[j];
        if(sign != 0x1F) continue;
        // Map the permutations to 5-9
        for (int j = 0; j < 5; j++) perm[j]+=5;
        int64_t num = test_permutation(perm);
        if (num > max)
        {
            printf("New max: %ld %ld %ld %ld %ld: %ld\n",
                   perm[0],
                   perm[1],
                   perm[2],
                   perm[3],
                   perm[4],
                   num);
            max = num;
        }
    }
    return 0;
}
