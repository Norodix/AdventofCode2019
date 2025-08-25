#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define WIDTH  25
#define HEIGHT  6

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

    int min_zeros = 9999;
    int num_zeros = 0;
    int buff[WIDTH*HEIGHT];
    int val = 0;

    while(!feof(f))
    {
        num_zeros = 0;
        for (int i = 0; i < WIDTH*HEIGHT; i++) {
            buff[i] = getc(f) - 0x30;
            if (!buff[i]) num_zeros++;
        }
        if (feof(f)) break;
        if (num_zeros < min_zeros)
        {
            min_zeros = num_zeros;
            int num1 = 0, num2 = 0;
            for (int i = 0; i < WIDTH*HEIGHT; i++) {
                switch (buff[i]) {
                    case 1:
                        num1++;
                        break;
                    case 2:
                        num2++;
                        break;
                    default:
                        break;
                }
            }
            val = num1 * num2;
            printf("Found new min %d, value: %d\n", min_zeros, val);
        }
    }
    printf("Found the value: %d\n", val);
    return 0;
}
