#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char** tiles;
#define MAXKEYS 26
char keys[MAXKEYS];
#define LINELEN 1000
#define MIN(a, b) (a < b ? a : b)

#define VISITED (1 << 7)

typedef struct node {
    int r; // Row
    int c; // Column
    int value; // How many steps it took to get here
} node;

void print_tiles(char** tiles, int rows, int cols)
{
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            printf("%c", tiles[r][c]);
        }
        printf("\n");
    }
}

void clear_tiles(char** t, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            t[r][c] &= (~VISITED);
        }
    }
}

int bfs_search(char** t, int rows, int cols, int from_row, int from_col, int to_row, int to_col) {
    int ret = -1;
    node *nodes = (node*)malloc(sizeof(node) * rows * cols);
    memset(nodes, 0, sizeof(node) * rows * cols);

    nodes[0].r = from_row;
    nodes[0].c = from_col;
    int write_index = 1;
    int read_index = 0;
    while (read_index < write_index) {
        node n = nodes[read_index++];
        if (n.r == to_row && n.c == to_col) {
            // Found
            ret = n.value;
            break;
        }
        // For all 4 neighbors
        node neighbors[4] = {
            { .r = n.r+1, .c = n.c,   },
            { .r = n.r-1, .c = n.c,   },
            { .r = n.r,   .c = n.c+1, },
            { .r = n.r,   .c = n.c-1, },
        };

        for (int i = 0; i < 4; i++) {
            node ne = neighbors[i];
            char c = t[ne.r][ne.c];
            if (c & VISITED) continue;
            if (c == '#') continue;
            if (c >= 'A' && c <= 'Z') continue;
            // valid, player, or empty or key
            ne.value = n.value + 1;
            nodes[write_index++] = ne;
            t[ne.r][ne.c] |= VISITED;
        }
    }

    clear_tiles(t, rows, cols);
    free(nodes);
    return ret;
}

node get_door_pos(char** t, int rows, int cols, char key) {
    node ret;
    ret.value = -1;
    char door = key + 'A' - 'a';
    // printf("Looking for %c\n", door);
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (t[r][c] == door) {
                ret.value = 1;
                ret.r = r;
                ret.c = c;
                // printf("Found at %d %d\n", r, c);
                // Break out of nested loop
                goto end;
            }
        }
    }
end:
    return ret;
}

// TODO memoize based on keys left on map
int solve(char** t, int rows, int cols, int start_row, int start_col) {
    char keys[MAXKEYS];
    int key_rows[MAXKEYS];
    int key_cols[MAXKEYS];
    int key_index = 0;
    memset(keys, 0, MAXKEYS);

    // Get keys left on the map and their positions
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            char k = t[r][c];
            if (k >= 'a' && k <= 'z') {
                keys[key_index] = k;
                key_rows[key_index] = r;
                key_cols[key_index] = c;
                key_index++;
            }
        }
    }
    printf("Keys left on map: %s\n", keys);
    // print_tiles(t, rows, cols);

    // Base case
    if (key_index == 0) return 0;

    int min_d = 9999999;

    for (int i = 0; i < key_index; i++) {
        int distance = bfs_search(t, rows, cols, start_row, start_col, key_rows[i], key_cols[i]);
        if (distance < 0) {
            // printf("Cannot reach %c\n", keys[i]);
            continue;
        }
        // remove key and door from tiles
        node door_pos = get_door_pos(t, rows, cols, keys[i]);
        t[key_rows[i]][key_cols[i]] = '.';
        if (door_pos.value > 0)
            t[door_pos.r][door_pos.c] = '.';

        int distance_rest = solve(t, rows, cols, key_rows[i], key_cols[i]);
        // printf("Distance to reach %c: %d, for the rest the minimum is %d\n", keys[i], distance, distance_rest);

        // re-add key and door to tiles
        t[key_rows[i]][key_cols[i]] = keys[i];
        if (door_pos.value > 0 )
            t[door_pos.r][door_pos.c] = keys[i] + 'A' - 'a';

        min_d = MIN(min_d, distance + distance_rest);
    }

    return min_d;
}


void test_bfs() {
    int rows = 10, cols = 10;
    char** t = malloc(sizeof(char*) * rows);
    for (int i = 0; i < cols; i++) {
        t[i] = malloc(sizeof(char) * cols);
    }
    char map[] = \
        "##########" \
        "#        #" \
        "#  #     #" \
        "#  #     #" \
        "#  ##### #" \
        "#  #     #" \
        "#  #     #" \
        "#  ####  #" \
        "#        #" \
        "##########" \
    ;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            t[r][c] = map[r*cols + c];
        }
    }

    print_tiles(t, rows, cols);

    printf("From %d,%d to %d,%d -> %d, expected  5\n", 1, 1, 3, 4, bfs_search(t, 10, 10, 1, 1, 3, 4));
    printf("From %d,%d to %d,%d -> %d, expected 10\n", 3, 4, 5, 4, bfs_search(t, 10, 10, 3, 4, 5, 4));
    printf("From %d,%d to %d,%d -> %d, expected 15\n", 1, 1, 5, 4, bfs_search(t, 10, 10, 1, 1, 5, 4));
    printf("From %d,%d to %d,%d -> %d, expected 15\n", 1, 1, 5, 4, bfs_search(t, 10, 10, 1, 1, 5, 4));
    printf("From %d,%d to %d,%d -> %d, expected 15\n", 1, 1, 5, 4, bfs_search(t, 10, 10, 1, 1, 5, 4));
    for (int i = 0; i < cols; i++) {
        free(t[i]);
    }
    free(t);
}

int main(int argc, char** argv) {
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

    // test_bfs();

    // Read file into memory
    int lines = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        lines++;
        fgets(line, LINELEN, f);
    }
    rewind(f);

    int rows = lines;
    int cols = strlen(line)-1;

    tiles = malloc(sizeof(char*) * rows);
    for (int i = 0; i < cols; i++) {
        tiles[i] = malloc(sizeof(char) * cols);
    }

    lines = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        memcpy(tiles[lines], line, cols);
        lines++;
        fgets(line, LINELEN, f);
    }

    // print_tiles(tiles, rows, cols);

    // Get starting position
    int start_row = 0;
    int start_col = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c] == '@')
            {
                start_row = r;
                start_col = c;
            }
        }
    }

    // recursively solve for picking up each key in any order
    int min_solution = solve(tiles, rows, cols, start_row, start_col);
    printf("Minimum solution found: %d\n", min_solution);

    return 0;
}

