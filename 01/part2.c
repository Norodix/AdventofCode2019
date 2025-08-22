#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINELEN (1024) // maximum length of lines to be read

int get_fuel(int mass)
{
    int total_fuel = 0;
    int unaccounted_mass = mass;
    while (unaccounted_mass > 0)
    {
        int fuel = unaccounted_mass / 3 - 2;
        if (fuel > 0)
        {
            total_fuel += fuel;
        }
        unaccounted_mass = fuel;
    }
    return total_fuel;
}

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
        sum += get_fuel(strtol(line, NULL, 10));
        fgets(line, sizeof(line), f);
    }

    printf("The fuel sum is: %d\n", sum);
    return 0;
}
