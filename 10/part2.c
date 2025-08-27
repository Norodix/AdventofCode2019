#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

#define LINELEN    (1024) // maximum length of lines to be read
#define ASTEROID   ('#')
#define EMPTY      ('.')
#define COUNTED    (0x80)
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#define PI 3.1415f

typedef struct asteroid {
    int row;
    int col;
} asteroid;

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

// Get the angle relative to directly up starting to the right
float get_angle(int r, int c) {
    if (r < 0 && c == 0) return 0;
    // row is negative because it is opposite
    float angle = atan2(-r, c);
    angle -= (PI/2.0);
    if (angle < -PI) angle += 2*PI;
    angle *= -1;
    angle += 2*PI;
    if (angle >= 2*PI) angle -= 2*PI;
    return angle;
}

int orig_r, orig_c;
float get_ast_angle(asteroid a)
{
    float angle = get_angle(a.row - orig_r, a.col - orig_c);
    angle = round(angle*10000.0)/10000.0;
    return angle;
}

float get_ast_dist(asteroid a)
{
    float r = a.row - orig_r;
    float c = a.col - orig_c;
    float dist = r*r + c*c;
    dist = round(dist*10000.0)/10000.0;
    return dist;
}

int compare_ast(const void* _a, const void* _b) {
    asteroid a = *(asteroid*)_a;
    asteroid b = *(asteroid*)_b;

    float angle_a = get_ast_angle(a) * 10000.0;
    float angle_b = get_ast_angle(b) * 10000.0;
    if (angle_a == angle_b) {
        float dist_a = get_ast_dist(a);
        float dist_b = get_ast_dist(b);
        return dist_a - dist_b;
    }
    return angle_a - angle_b;
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

void test_angle() {
    int numpairs[][2] = 
        {
            -1, 0,
            -1, 1,
            0, 1,
            1, 1, // down right
            1, 0,
            1, -1,
            0, -1,
            -1, -1, // up left
        };

    for (int i = 0; i < sizeof(numpairs) / sizeof(numpairs[0]); i++) {
        int a = numpairs[i][0];
        int b = numpairs[i][1];
        printf("gcd(%d, %d) = %f\n", a, b, get_angle(a, b));
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
    int maxr, maxc;
    // Go through each tile and calculate the score
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            clear_map(tiles, rows, cols);
            if(tiles[r][c] != ASTEROID) continue;
            for (int vr = 0; vr < rows; vr++) {
                for (int vc = 0; vc < cols; vc++) {
                    if (tiles[vr][vc] != ASTEROID) continue;
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
                maxr = r;
                maxc = c;
                max_score = score;
            }
        }
    }
    printf("Max score: %d\n", max_score);
    orig_r = maxr;
    orig_c = maxc;

    clear_map(tiles, rows, cols);
    // TODO loop through asteroids
    // Check that it is indeed in line of sight -> 1 to div there is no other asteroid
    // save into array
    // Sort it by angle

    // OR

    // Parse each asteroid into struct array
    int num_asteroids = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (r == orig_r && c == orig_c) continue;
            if (tiles[r][c] == ASTEROID) num_asteroids++;
        }
    }
    asteroid *asteroids = malloc(sizeof(asteroid) * num_asteroids);
    int index = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (r == orig_r && c == orig_c) continue;
            if (tiles[r][c] != ASTEROID) continue;
            asteroids[index++] = (asteroid) {
                .col = c,
                .row = r,
            };
        }
    }
    // sort them by angle and if angle
    // calculating angle so that it is indeed from north going east
    qsort(asteroids, num_asteroids, sizeof(asteroid), compare_ast);
    for (int i = 0; i < num_asteroids; i++) {
        asteroid a = asteroids[i];
        tiles[a.row][a.col] = 'A' + i/20;
        // printf("%c: %f\n", 'A' + i, get_ast_angle(a));
    }
    tiles[orig_r][orig_c] = '#';
    print_map(tiles, rows, cols);

    // go through all nodes, if angles are the same then push the later to the very end of the queue
    for (int i = 0; i < num_asteroids - 1; i++) {
        // up to n times, where n is the remaining number of asteroids
        int n = num_asteroids - i - 1;
        while(--n){
            float angle_this = get_ast_angle(asteroids[i]);
            float angle_next = get_ast_angle(asteroids[i+1]);
            if (angle_this == angle_next) {
                asteroid temp = asteroids[i+1];
                memmove(asteroids+i+1, asteroids+i+2, (num_asteroids - i - 2) * sizeof(asteroid));
                asteroids[num_asteroids-1] = temp;
            }
            else
            break;
        }
    }

    for (int i = 0; i < num_asteroids; i++) {
        asteroid a = asteroids[i];
        tiles[a.row][a.col] = 'A' + i/20;
    }
    tiles[orig_r][orig_c] = '#';
    print_map(tiles, rows, cols);

    printf("The 1st asteroid is: (%d, %d)\n", asteroids[0].row, asteroids[0].col);
    printf("The 200th asteroid is: (%d, %d)\n", asteroids[199].row, asteroids[199].col);

    printf("The score is %d\n", asteroids[199].col * 100 + asteroids[199].row);

    return 0;
}
