#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define LINELEN 4000

void transform_array(int* arr, int len) {
    for (int i = 1; i < len; i++) {
        arr[i] = arr[i-1] + arr[i];
        arr[i] %= 10;
    }
}

void print_digits(int* arr, int len) {
    int n = 24; // how many digits to print on either side
    for (int i = 0; i < n; i++) {
        int digit = arr[i];
        printf("%d ", digit);
    }
    printf(" ... ");
    for (int i = len-n; i < len; i++) {
        int digit = arr[i];
        printf("%d ", digit);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    int* multiplier_array;
    char line[LINELEN];
    char index[10];
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
    if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
    strncpy(index, line, 7); // first 7 digits, 8 long because last one is \0
    index[7] = '\0';

    int64_t offset = strtol(index, NULL, 10);
    printf("Offset is %d\n", offset);
    int64_t len = strlen(line) * 10000 - offset;
    printf("The length of the stored multiplier array is %ld out of a total of %ld\n", len, strlen(line)*10000);
    // set the multiplier array to default
    multiplier_array = (int*) malloc(sizeof(int) * len);
    for (int i = 0; i < len; i++) {
        int input_array_index = strlen(line) * 10000 - i - 1;
        int line_index = (input_array_index) % strlen(line);
        multiplier_array[i] = (line[line_index] - '0');
        multiplier_array[i] %= 10;
    }
    print_digits(multiplier_array, len);

    for (int i = 0; i < 100; i++) {
        transform_array(multiplier_array, len);
        print_digits(multiplier_array, len);
    }

    printf("Solution: ");
    for (int i = 0; i < 8; i++) {
        printf("%d", multiplier_array[len-1-i]);
    }
    printf("\n");


    return 0;
}

