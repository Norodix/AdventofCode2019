#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LINELEN (0x4000) // maximum length of lines to be read
#define GRIDSIZE 35000
#define CENTER (GRIDSIZE / 2)
static int grid[GRIDSIZE][GRIDSIZE][2];
static int rowmin = CENTER;
static int colmin = CENTER;
static int rowmax = CENTER;
static int colmax = CENTER;
#define MIN(x,y) (x<y ? x : y)
#define MAX(x,y) (x>y ? x : y)

void print_center(int size)
{
    for (int index = 0; index <= 1; index++){
        for (int row = CENTER-size; row < CENTER + size; row++)
        {
            for (int col = CENTER-size; col < CENTER + size; col++)
            {
                printf("%2d ", grid[row][col][index]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    memset(grid, 0, sizeof(grid));
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

    for (int i = 0; i <= 1; i++){
        int distance = 0;
        int row = CENTER, col = CENTER;
        fgets(line, sizeof(line), f);
        char* token = strtok(line, ",");
        while(token != NULL)
        {
            char dir = token[0];
            int len = strtol(token+1, NULL, 10);
            printf("%d %c %d\n", i, dir, len);
            for (int j = 0; j < len; j++){
                distance++;
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
                grid[row][col][i] = distance;
                printf("Setting %d at %d %d\n", distance, row, col);
            }
            token = strtok(NULL, ",");
        }
    }

    print_center(10);
    printf("Bounds: %d - %d     %d - %d\n", rowmin, rowmax, colmin, colmax);

    int dmin = GRIDSIZE;
    for (int row = rowmin; row <= rowmax; row++){
        for (int col = colmin; col <= colmax; col++){
            if (grid[row][col][0] && grid[row][col][1]){
                int d = grid[row][col][0] + grid[row][col][1];
                if (d < dmin) {
                    printf("Minimal found at %d %d with values %d %d\n", row, col, grid[row][col][0], grid[row][col][1]);
                }
                dmin = MIN(d, dmin);
            }
        }
    }

    printf("Minimum signal distance is %d\n", dmin);

    return 0;
}
