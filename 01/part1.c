#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINELEN (1024) // maximum length of lines to be read

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Must have 1 arg, the input file name\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL){
        printf("Could not open file\n");
        return -2;
    }
    char line[1024];
    fgets(line, sizeof(line), f);

    int sum = 0;
    while (!feof(f)) {
        sum += strtol(line, NULL, 10) / 3 - 2;
        fgets(line, sizeof(line), f);
    }

    printf("The fuel sum is: %d\n", sum);
    return 0;
}
