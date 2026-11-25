#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define LINELEN 1000
#define MIN(a, b) (a < b ? a : b)
#define MAX_NEIGHBORS 10
#define MAX_NODES 100

struct graph_node {
    char name[3];
    int r;
    int c;
    int num_neighbors;
    int neighbors[MAX_NEIGHBORS];
    int neighbor_dist[MAX_NEIGHBORS];
    int jump_neighbor;
};

static int rows, cols;
static char* tiles;

// Nodes are indexed in arbitrary discovery order.
// Their index is their primary id.
struct graph_node nodes[MAX_NODES];
static int num_nodes = 0;

int is_shortcut_char(char c) {
    return (c <= 'Z' && c >= 'A');
}

int is_outer_layer(int r, int c) {
    if (r == 2) return 1;
    if (c == 2) return 1;
    if (rows - r == 2) return 1;
    if (cols - c == 2) return 1;
    return 0;
}

void print_tiles()
{
    // Top number row
    printf("    ");
    for (int c = 0; c < cols; c++) {
        printf("%c", c/10 ? c/10 + '0' : ' ');
    }
    printf("\n");
    printf("    ");
    for (int c = 0; c < cols; c++) {
        printf("%1d", c%10);
    }
    printf("\n");

    for (int r = 0; r < rows; r++) {
        printf("%3d ", r);
        for (int c = 0; c < cols; c++) {
            char pchar = tiles[r * cols + c];
            printf("%c", pchar);
        }
        printf("\n");
    }
    printf("\n");
}


// TODO
// Function to search through the nodes and find the one with the matching name

// Flood fill to get all the distances to all other nodes
struct fifo {
    int *points;
    int write_index; // this has been written
    int read_index; // this is to be read
};

void flood_fill(uint32_t* field, struct fifo f) {
    fflush(stdout);
    while (f.read_index <= f.write_index) {
        int p = f.points[f.read_index];
        // get the four neighbors
        int indexes[4] = { p+cols, p-cols, p+1, p-1 };
        // If unvisited and steppable
        for (int i = 0; i < 4; i++) {
            int index = indexes[i];
            if (tiles[index] == '.' && field[index] == UINT32_MAX) {
                // distance is current +1
                field[index] = field[p]+1;
                // append the visited point to the fifo
                f.points[++f.write_index] = index;
            }
        }
        // This point has been dealt with
        f.read_index++;
    }
}

void print_distances(uint32_t* d) {
    for (int r = 0; r < rows; r++) {
        for(int c = 0; c < cols; c++) {
            int i = r*cols+c;
            if (d[i] != UINT32_MAX) {
                printf("%1u", d[i]%10);
            }
            else if(tiles[i] == '#'){
                printf(".");
            }
            else printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

void get_distances(int id) {
    uint32_t *d = malloc(sizeof(uint32_t)*rows*cols);
    memset(d, 0xFF, sizeof(uint32_t) * rows * cols);
    struct fifo f;
    f.points = (int*)malloc(sizeof(int) * rows * cols);
    f.write_index = 0;
    f.read_index = 0;
    f.points[0] = nodes[id].r * cols + nodes[id].c;

    // start is 0 steps
    d[nodes[id].r*cols+nodes[id].c] = 0;
    flood_fill(d, f);
    // printf("Distance field of %d\n\n\n", id);
    // print_distances(d);

    // Update the graph with distances
    for (int n = 0; n < num_nodes; n++) {
        if (n==id) continue;
        int n_pos = nodes[n].r*cols+nodes[n].c;
        if (d[n_pos] != UINT32_MAX) {
            struct graph_node *node = &nodes[id];
            node->neighbors[node->num_neighbors] = n;
            node->neighbor_dist[node->num_neighbors] = d[n_pos];
            node->num_neighbors++;
        }
    }
    printf("found %d neighbors\n", nodes[id].num_neighbors);
    free(d);
    free(f.points);
}

// TODO turn shortcut names to graph nodes
void generate_named_nodes() {
    // Match up shortcut designators
    // shortcut designations are always left to right top to bottom the same
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            char shortcut_name[3] = "";
            if (tiles[r * cols + c] == '.') {
                // check 4 surrounding tiles
                if (is_shortcut_char(  tiles[(r-1)*cols+c])) {
                    shortcut_name[0] = tiles[(r-2)*cols+c];
                    shortcut_name[1] = tiles[(r-1)*cols+c];
                }
                if (is_shortcut_char(  tiles[(r+1)*cols+c])) {
                    shortcut_name[0] = tiles[(r+1)*cols+c];
                    shortcut_name[1] = tiles[(r+2)*cols+c];
                }
                if (is_shortcut_char(  tiles[r*cols+c-1])) {
                    shortcut_name[0] = tiles[r*cols+c-2];
                    shortcut_name[1] = tiles[r*cols+c-1];
                }
                if (is_shortcut_char(  tiles[r*cols+c+1])) {
                    shortcut_name[0] = tiles[r*cols+c+1];
                    shortcut_name[1] = tiles[r*cols+c+2];
                }
                if (strlen(shortcut_name)) {
                    printf("Shortcut %s at %d,%d\n", shortcut_name, r, c);
                    // append node to the list
                    nodes[num_nodes] = (struct graph_node) {
                        .r = r,
                        .c = c,
                    };
                    memcpy(nodes[num_nodes].name, shortcut_name, 3);
                    num_nodes++;
                }
            }
        }
    }
}

// or get some insight where we can go with different chains
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

    rows = lines;
    cols = strlen(line)-1;

    tiles = (char*)malloc(sizeof(char) * rows * cols);

    lines = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        for (int i = 0; i < cols; i++) {
            tiles[lines * cols + i] = line[i];
        };
        lines++;
        fgets(line, LINELEN, f);
    }

    generate_named_nodes();


    print_tiles();

    for (int i = 0; i < num_nodes; i++) {
        get_distances(i);
    }

    // Look for the pairs and record them
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            if (i==j) continue;
            if (!strncmp(nodes[i].name, nodes[j].name, 3))
            {
                nodes[i].jump_neighbor = j;
                nodes[j].jump_neighbor = i;
            }
        }
    }


    // With the graph complete do a dijkstra
    // Make the depth of the graph dynamic and extend it when necessary
    // When getting the neighbors add the pair node at one level deeper/higher


    free(tiles);
    return 0;
}

