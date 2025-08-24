#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// special case, it can be only 1 computer and 1 ringbuffer that is its own in and output
static computer c;
static ringbuffer r;

int64_t test_permutation(computer* c, int64_t *phase_seq)
{
    int64_t res = 0;
    reset_buffer(c->in_buffer);

    for (int i = 0; i < 5; i++)
    {
        reset_memory(c);
        ring_push(c->in_buffer, phase_seq[i]);
        ring_push(c->in_buffer, res);
        process(c);
        ring_pop(c->out_buffer, &res);
    }
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
        reset_memory(&c);
        fgets(line, sizeof(line), f);
    }
    reset_memory(&c);
    c.out_buffer = &r;
    c.in_buffer = &r;

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
        int64_t num = test_permutation(&c, perm);
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
