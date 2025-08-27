#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define LINELEN    (1024) // maximum length of lines to be read
#define ASTERODID  ('#')
#define EMPTY      ('.')
#define COUNTED    (0x80)
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

int gcd(int a, int b)
{
    int temp;
    while (b != 0)
    {
        temp = a % b;

        a = b;
        b = temp;
    }
    a = a < 0 ? -a : a;
    return a;
}

void print_map(char** tiles, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        printf("%2i ", r);
        for (int c = 0; c < cols; c++) {
            printf("%c", tiles[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

int coords_valid(int rows, int cols, int r, int c) {
    if (r >= rows || r < 0) return 0;
    if (c >= cols || c < 0) return 0;
    return 1;
}

void clear_map(char** tiles, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            tiles[r][c] &= ~COUNTED;
        }
    }
}

int count_map(char** tiles, int rows, int cols) {
    int score = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            score += tiles[r][c] & COUNTED ? 1 : 0;
        }
    }
    return score;
}

void test_gcd() {
    int numpairs[][2] = 
        {
            1, 1,
            2,  4,
            1,  4,
            -1, 4
            -2, -4,
            2,  -4
        };

    for (int i = 0; i < sizeof(numpairs) / sizeof(numpairs[0]); i++) {
        int a = numpairs[i][0];
        int b = numpairs[i][1];
        printf("gcd(%d, %d) = %d\n", a, b, gcd(a, b));
    }
}

int main(int argc, char** argv) {
    int cols, rows;
    char** tiles;
    char line[LINELEN];
    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }

    fgets(line, LINELEN, f);
    cols = strlen(line) - 1;
    rows = 0;
    while (!feof(f)) {
        rows++;
        fgets(line, LINELEN, f);
    }
    rewind(f);

    // Allocate array to store the tiles
    tiles = (char**) malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++)
    {
        tiles[i] = (char*) malloc(cols * sizeof(int));
    }

    // Initialize the tiles
    fgets(line, LINELEN, f);
    int r = 0;
    while (!feof(f)) {
        for (int c = 0; c < strlen(line) - 1; c++) {
            tiles[r][c] = line[c];
        }
        r++;
        fgets(line, LINELEN, f);
    }
    print_map(tiles, rows, cols);
    test_gcd();

    int max_score = 0;
    // Go through each tile and calculate the score
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            clear_map(tiles, rows, cols);
            if(tiles[r][c] != ASTERODID) continue;
            for (int vr = 0; vr < rows; vr++) {
                for (int vc = 0; vc < cols; vc++) {
                    if (tiles[vr][vc] != ASTERODID) continue;
                    int dr = vr - r;
                    int dc = vc - c;
                    if (!(dr || dc)) {
                        // tiles[r][c] |= COUNTED;
                        continue;
                    }
                    int div = gcd(dr, dc);
                    dr /= div;
                    dc /= div;
                    tiles[r+dr][c+dc] |= COUNTED;
                }
            }
            int score = count_map(tiles, rows, cols);
            if (score > max_score) {
                printf("New max found at %d %d: %d\n", r, c, score);
                max_score = score;
            }
        }
    }

    printf("Max score: %d\n", max_score);
    return 0;
}
