#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LINELEN (0x4000) // maximum length of lines to be read
#define GRIDSIZE 35000
#define CENTER (GRIDSIZE / 2)
static int grid[GRIDSIZE][GRIDSIZE] = {0};
static int rowmin = CENTER;
static int colmin = CENTER;
static int rowmax = CENTER;
static int colmax = CENTER;
#define MIN(x,y) (x<y ? x : y)
#define MAX(x,y) (x>y ? x : y)

void print_center(int size)
{
    for (int row = CENTER-size; row < CENTER + size; row++)
    {
        for (int col = CENTER-size; col < CENTER + size; col++)
        {
            printf("%c", grid[row][col] ? grid[row][col] + 0x30 : '.');
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    // memset(grid, 0, sizeof(grid));
    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }
    char line[LINELEN];

    int row = CENTER, col = CENTER;
    for (int i = 1; i <= 2; i++){
        row = CENTER;
        col = CENTER;
        fgets(line, sizeof(line), f);
        char* token = strtok(line, ",\n");
        while(token != NULL)
        {
            char dir = token[0];
            int len = strtol(token+1, NULL, 10);
            printf("%d %c %d\n", i, dir, len);
            for (int j = 0; j < len; j++){
                switch (dir) {
                    case 'R':{
                        col++;
                        colmax = MAX(col, colmax);
                        break;
                    }
                    case 'L':{
                        col--;
                        colmin = MIN(col, colmin);
                        break;
                    }
                    case 'U':{
                        row--;
                        rowmin = MIN(row, rowmin);
                        break;
                    }
                    case 'D':{
                        row++;
                        rowmax = MAX(row, rowmax);
                        break;
                    }
                    default: {
                        printf("Invalid direction\n");
                        return -1;
                    }
                }
                if (MAX(row, col) > GRIDSIZE || MIN(row, col) < 0)
                {
                    printf("Out of bounds!!!\n");
                    return -1;
                }
                grid[row][col] |= i;
            }
            token = strtok(NULL, ",\n");
        }
    }

    print_center(10);
    printf("Bounds: %d - %d     %d - %d\n", rowmin, rowmax, colmin, colmax);

    // Do not check everything, just follow one line through
    int dmin = GRIDSIZE;
    rewind(f);
    fgets(line, sizeof(line), f);
    char* token = strtok(line, ",\n");
    row = CENTER;
    col = CENTER;
    while(token != NULL)
    {
        char dir = token[0];
        int len = strtol(token+1, NULL, 10);
        for (int j = 0; j < len; j++){
            switch (dir) {
                case 'R':{
                    col++;
                    break;
                }
                case 'L':{
                    col--;
                    break;
                }
                case 'U':{
                    row--;
                    break;
                }
                case 'D':{
                    row++;
                    break;
                }
                default: {
                    printf("Invalid direction\n");
                    return -1;
                }
            }
            if (grid[row][col] == 3){
                int d = abs(row-CENTER) + abs(col-CENTER);
                dmin = MIN(d, dmin);
            }
        }
        token = strtok(NULL, ",\n");
    }
    printf("Minimum distance is %d\n", dmin);

    return 0;
}
