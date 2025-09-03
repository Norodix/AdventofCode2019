#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define LINELEN 1000
#define LAYERS 100000 // sure we are not gonna go more than a 100 layers deep, that would be stupid
#define MIN(a, b) (a < b ? a : b)

typedef struct node {
    int r; // Row
    int c; // Column
    int layer; // we have to go deeper
    char tile; // tileface
    int visited; // Has this tile been visited
    int value; // How many steps it took to get here
    struct node *teleport; // Where this node can teleport
} node;

int is_shortcut_char(char c) {
    return (c <= 'Z' && c >= 'A');
}

node *shortcuts[26*26*2];
int get_shortcut_index(char name[2]) {
    int a = name[0] - 'A';
    int b = name[1] - 'A';
    return (a*26+b) * 2;
}

node*** tiles;

int is_outer_layer(node *n, int rows, int cols) {
    if (n->r == 2) return 1;
    if (n->c == 2) return 1;
    if (rows - n->r == 2) return 1;
    if (cols - n->c == 2) return 1;
    return 0;
}

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
            char pchar = tiles[r][c].tile;
            if (tiles[r][c].visited) pchar = 'X';
            if (tiles[r][c].teleport != NULL) pchar = '@';
            printf("%c", pchar);
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

int bfs_search(node*** t, int rows, int cols, node* from, node* to) {
    static int maxdepth = 0;
    int ret = -1;
    // Absolute maximum possible nodes to visit
    node **nodes = (node**)malloc(sizeof(node*) * rows * cols * LAYERS);
    memset(nodes, 0, sizeof(node*) * rows * cols * LAYERS);

    nodes[0] = from;
    nodes[0]->visited = 1;
    int write_index = 1;
    int read_index = 0;
    while (read_index < write_index) {
        node *n = nodes[read_index++];
        if (n->r == to->r &&
            n->c == to->c &&
            n->layer == to->layer) {
            // Found
            ret = n->value;
            break;
        }
        if (n->layer > maxdepth) {
            printf("depth: %d\n", n->layer);
            maxdepth = n->layer;
        }

        // For all 5 neighbors
        node *neighbors[5] = {
            &tiles[n->layer][n->r + 1 ][n->c     ],
            &tiles[n->layer][n->r - 1 ][n->c     ],
            &tiles[n->layer][n->r     ][n->c + 1 ],
            &tiles[n->layer][n->r     ][n->c - 1 ],
            n->teleport,
        };

        for (int i = 0; i < 5; i++) {
            node *ne = neighbors[i];
            if (ne == NULL) continue;
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

    tiles = malloc(sizeof(node**) * LAYERS);
    for (int l = 0; l < LAYERS; l++) {
        tiles[l] = malloc(sizeof(node*) * rows);
        for (int r = 0; r < rows; r++) {
            tiles[l][r] = malloc(sizeof(node) * cols);
        }
    }

    lines = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        for (int i = 0; i < cols; i++) {
            tiles[0][lines][i] = (node) {
                .r = lines,
                .c = i,
                .tile = line[i],
                .visited = 0,
                .value = 0,
                .layer = 0,
                .teleport = NULL,
            };
        }
        lines++;
        fgets(line, LINELEN, f);
    }

    // update all tiles to correct layers
    for (int l = 1; l < LAYERS; l++) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                memcpy(&tiles[l][r][c], &tiles[0][r][c], sizeof(node));
                tiles[l][r][c].layer = l;
            }
        }
    }

    memset(shortcuts, 0, sizeof(shortcuts));

    // Match up shortcut designators
    // shortcut designations are always left to right top to bottom the same
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            char shortcut_name[3] = "";
            node** t = tiles[0];
            if (t[r][c].tile == '.') {
                // check 4 surrounding tiles
                if (is_shortcut_char(t[r-1][c].tile)) {
                    shortcut_name[0] = t[r-2][c].tile;
                    shortcut_name[1] = t[r-1][c].tile;
                }
                if (is_shortcut_char(t[r+1][c].tile)) {
                    shortcut_name[0] = t[r+1][c].tile;
                    shortcut_name[1] = t[r+2][c].tile;
                }
                if (is_shortcut_char(t[r][c-1].tile)) {
                    shortcut_name[0] = t[r][c-2].tile;
                    shortcut_name[1] = t[r][c-1].tile;
                }
                if (is_shortcut_char(t[r][c+1].tile)) {
                    shortcut_name[0] = t[r][c+1].tile;
                    shortcut_name[1] = t[r][c+2].tile;
                }
                if (strlen(shortcut_name)) {
                    // printf("Shortcut %s at %d,%d\n", shortcut_name, r, c);
                    int idx = get_shortcut_index(shortcut_name);
                    if (shortcuts[idx] != NULL) {
                        idx++;
                    }
                    shortcuts[idx] = &t[r][c];
                }
            }
        }
    }

    // TODO matching up does not really work. Maybe I mess up shortcut/tile pointers somewhere
    for (int idx = 0; idx < sizeof(shortcuts)/sizeof(shortcuts[0]); idx += 2) {
        if (!shortcuts[idx]) continue;
        if (!shortcuts[idx+1]) {
            printf("Skipping %d,%d\n", shortcuts[idx]->r, shortcuts[idx]->c);
            continue; // this filters for AA and ZZ
        }
        node *outer, *inner;
        if (is_outer_layer(shortcuts[idx], rows, cols)) {
            outer = shortcuts[idx];
            inner = shortcuts[idx+1];
        }
        else {
            outer = shortcuts[idx+1];
            inner = shortcuts[idx];
        }
        printf("Inner: %d,%d\t Outer: %d,%d\n", inner->r, inner->c, outer->r, outer->c);

        for (int l = 0; l < LAYERS - 1; l++) {
            // Connect the upper level (lower number) inside node to the lower level outside node
            node* upper = &tiles[l  ][inner->r][inner->c];
            node* lower = &tiles[l+1][outer->r][outer->c];
            upper->teleport = lower;
            lower->teleport = upper;
        }
    }

    print_tiles(tiles[1], rows, cols);

    node* startnode = shortcuts[get_shortcut_index("AA")];
    startnode = &tiles[0][startnode->r][startnode->c];
    node* endnode = shortcuts[get_shortcut_index("ZZ")];
    endnode = &tiles[0][endnode->r][endnode->c];
    printf("Starting search\n");
    int min_distance = bfs_search(tiles, rows, cols, startnode, endnode);
    printf("Minimum distance found: %d\n", min_distance);
    print_tiles(tiles[0], rows, cols);
    print_tiles(tiles[1], rows, cols);

    return 0;
}

