#include <comp.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// local buffer that can temporarily store the in and outputs
static int64_t input_buffer[2];
static int input_index = 0;
static int64_t output_buffer;
static computer c;
// Define these two here to allow chaining the program
int64_t get_custom_input()
{
    int64_t ret = input_buffer[input_index++];
    input_index %= 2;
    return ret;
}

void put_custom_output(int64_t num)
{
    output_buffer = num;
}

int64_t test_permutation(int64_t *phase_seq)
{
    output_buffer = 0;
    input_index = 0;
    for (int i = 0; i < 5; i++)
    {
        reset_memory(&c);
        input_buffer[0] = phase_seq[i];
        input_buffer[1] = output_buffer;
        process(&c);
    }
    return output_buffer;
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
