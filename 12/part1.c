#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define LINELEN 1000

typedef struct moon {
    int x, y, z;
    int vx, vy, vz;
} moon;

#define SIGN(x) (x < 0 ? -1 : x > 0 ? 1 : 0)

// TODO moons should be stepped in a way that it preserves their position during the timestep
// wait, we are doing velocity first, so it is automatically true
void step_moons(moon *m, int len) {
    for (int i = 0; i < len; i++)
    {
        for (int j = i+1; j < len; j++) 
        {
            int dx = SIGN(m[j].x - m[i].x);
            m[i].vx += dx;
            m[j].vx -= dx;
            int dy = SIGN(m[j].y - m[i].y);
            m[i].vy += dy;
            m[j].vy -= dy;
            int dz = SIGN(m[j].z - m[i].z);
            m[i].vz += dz;
            m[j].vz -= dz;
        }
    }
    for (int i = 0; i < len; i++)
    {
        m[i].x += m[i].vx;
        m[i].y += m[i].vy;
        m[i].z += m[i].vz;
    }
}

void print_moons(moon *m, int len) {
    for (int i = 0; i < len; i++)
    {
        printf("pos=<x=%3d, y=%3d, z=%3d>, vel=<x=%3d, y=%3d, z=%3d>\n", \
               m[i].x,  \
               m[i].y,  \
               m[i].z,  \
               m[i].vx, \
               m[i].vy, \
               m[i].vz  \
               );
    }
    printf("\n");
}

int moons_energy(moon *m, int len) {
    int e = 0;
    int ke, pe;
    for (int i = 0; i < len; i++)
    {
        ke = 0; pe = 0;
        ke += abs(m[i].x);
        ke += abs(m[i].y);
        ke += abs(m[i].z);
        pe += abs(m[i].vx);
        pe += abs(m[i].vy);
        pe += abs(m[i].vz);
        e += ke*pe;
    }
    return e;
}

int main(int argc, char** argv) {
    moon *moons;
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

    int lines = 0;
    fgets(line, LINELEN, f);
    while (!feof(f)) {
        lines++;
        fgets(line, LINELEN, f);
    }
    rewind(f);

    moons = (moon*)malloc(sizeof(moon) * lines);
    memset(moons, 0, sizeof(moon)*lines);

    fgets(line, LINELEN, f);
    int i = 0;
    while (!feof(f)) {
        sscanf(line, "<x=%d, y=%d, z=%d>\n", &moons[i].x, &moons[i].y, &moons[i].z);
        i++;
        fgets(line, LINELEN, f);
    }

    for (int i = 0; i < lines; i++)
    {
        moon *m = &moons[i];
        printf("%d %d %d\n", m->x, m->y, m->z);
    }

    for (i = 0; i < 1000; i++)
    {
        // if (!(i%10))
        // {
        //     printf("After %d steps:\n", i);
        //     print_moons(moons, lines);
        // }
        step_moons(moons, lines);
    }
    printf("After %d steps:\n", i);
    print_moons(moons, lines);

    printf("Energy of system: %d\n", moons_energy(moons, lines));
}

