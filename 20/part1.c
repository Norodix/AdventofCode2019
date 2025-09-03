#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define LINELEN 1000
#define MIN(a, b) (a < b ? a : b)

typedef struct node {
    int r; // Row
    int c; // Column
    char tile; // tileface
    int visited; // Has this tile been visited
    int value; // How many steps it took to get here
    int tele_r;
    int tele_c;
} node;

int is_shortcut_char(char c) {
    return (c <= 'Z' && c >= 'A');
}

int get_shortcut_index(char name[2]) {
    int a = name[0] - 'A';
    int b = name[1] - 'A';
    return (a*26+b);
}
node shortcuts[26*26];

node** tiles;

void print_tiles(node** tiles, int rows, int cols)
{
    // Top number row
    printf("   ");
    for (int c = 0; c < cols; c++) {
        printf("%c", c/10 ? c/10 + '0' : ' ');
    }
    printf("\n");
    printf("   ");
    for (int c = 0; c < cols; c++) {
        printf("%1d", c%10);
    }
    printf("\n");

    for (int r = 0; r < rows; r++) {
        printf("%2d ", r);
        for (int c = 0; c < cols; c++) {
            printf("%c", tiles[r][c].tile);
        }
        printf("\n");
    }
    printf("\n");
}

void clear_tiles(node** t, int rows, int cols) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            t[r][c].visited = 0;
        }
    }
}

int bfs_search(node** t, int rows, int cols, node* from, node* to) {
    int ret = -1;
    node **nodes = (node**)malloc(sizeof(node*) * rows * cols);
    memset(nodes, 0, sizeof(node*) * rows * cols);

    nodes[0] = from;
    int write_index = 1;
    int read_index = 0;
    while (read_index < write_index) {
        node *n = nodes[read_index++];
        if (n->r == to->r && n->c == to->c) {
            // Found
            ret = n->value;
            break;
        }

        // For all 4 neighbors
        node *neighbors[5] = {
            &tiles[n->r + 1 ][n->c     ],
            &tiles[n->r - 1 ][n->c     ],
            &tiles[n->r     ][n->c + 1 ],
            &tiles[n->r     ][n->c - 1 ],
            &tiles[n->tele_r][n->tele_c],
        };

        for (int i = 0; i < 5; i++) {
            node *ne = neighbors[i];
            if (ne->visited) continue;
            if (ne->tile != '.') continue;
            // valid neighbor
            ne->value = n->value + 1;
            nodes[write_index++] = ne;
            ne->visited = 1;
        }
    }
    printf("Before free we got up to %d nodes\n", write_index);
    free(nodes);
    return ret;
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

    tiles = malloc(sizeof(node*) * rows);
    for (int i = 0; i < rows; i++) {
        tiles[i] = malloc(sizeof(node) * cols);
    }

    lines = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        for (int i = 0; i < cols; i++) {
            tiles[lines][i] = (node) {
                .r = lines,
                .c = i,
                .tile = line[i],
                .visited = 0,
                .value = 0,
            };
        }
        lines++;
        fgets(line, LINELEN, f);
    }

    memset(shortcuts, 0, sizeof(shortcuts));

    // Match up shortcut designators
    // shortcut designations are always left to right top to bottom the same
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            char shortcut_name[3] = "";
            if (tiles[r][c].tile == '.') {
                // check 4 surrounding tiles
                if (is_shortcut_char(tiles[r-1][c].tile)) {
                    shortcut_name[0] = tiles[r-2][c].tile;
                    shortcut_name[1] = tiles[r-1][c].tile;
                }
                if (is_shortcut_char(tiles[r+1][c].tile)) {
                    shortcut_name[0] = tiles[r+1][c].tile;
                    shortcut_name[1] = tiles[r+2][c].tile;
                }
                if (is_shortcut_char(tiles[r][c-1].tile)) {
                    shortcut_name[0] = tiles[r][c-2].tile;
                    shortcut_name[1] = tiles[r][c-1].tile;
                }
                if (is_shortcut_char(tiles[r][c+1].tile)) {
                    shortcut_name[0] = tiles[r][c+1].tile;
                    shortcut_name[1] = tiles[r][c+2].tile;
                }
                if (strlen(shortcut_name)) {
                    // printf("Shortcut %s at %d,%d\n", shortcut_name, r, c);
                    int idx = get_shortcut_index(shortcut_name);
                    if (shortcuts[idx].tile == '.') {
                        // Already know of a shortcut with this name, add each other as teleports
                        int other_r = shortcuts[idx].r;
                        int other_c = shortcuts[idx].c;
                        tiles[other_r][other_c].tele_r = r;
                        tiles[other_r][other_c].tele_c = c;
                        tiles[r][c].tele_r = other_r;
                        tiles[r][c].tele_c = other_c;
                    }
                    else {
                        // This shortcut has no pair yet, save it for later
                        shortcuts[idx] = tiles[r][c];
                    }
                }
            }
        }
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (tiles[r][c].tele_r != 0) {
                printf("Teleport from %d,%d to %d, %d\n", r, c, tiles[r][c].tele_r, tiles[r][c].tele_c);
            }
        }
    }

    print_tiles(tiles, rows, cols);

    // Trick to never visit tile[0][0]
    // This way we don't have to check if node has teleport neighbor, because all teleport
    // neighbors will point here which they will skip because it appears visited
    tiles[0][0].visited = 1;
    node* startnode = &shortcuts[get_shortcut_index("AA")];
    startnode = &tiles[startnode->r][startnode->c];
    node* endnode = &shortcuts[get_shortcut_index("ZZ")];
    endnode = &tiles[endnode->r][endnode->c];
    printf("Starting from %d,%d\n", startnode->r, startnode->c);
    int min_distance = bfs_search(tiles, rows, cols, startnode, endnode);
    printf("Minimum distance found: %d\n", min_distance);

    return 0;
}

