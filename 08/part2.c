#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define WIDTH  25
#define HEIGHT  6

#define BLACK 0
#define WHITE 1
#define TRANS 2

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
    for (int i = 0; i < WIDTH*HEIGHT; i++)
    {
        buff[i] = 2;
    }

    while(!feof(f))
    {
        num_zeros = 0;
        for (int i = 0; i < WIDTH*HEIGHT; i++) {
            int col = getc(f) - 0x30;
            buff[i] = buff[i] == TRANS ? col : buff[i];
            if (feof(f)) break;
        }
    }
    for (int row = 0; row < HEIGHT; row++)
    {
        for (int col = 0; col < WIDTH; col++)
        {
            printf("%c", buff[col+row*WIDTH] == WHITE ? '#' : ' ');
        }
        printf("\n");
    }
    return 0;
}
