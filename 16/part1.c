#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define LINELEN 4000

int basepattern[] = {0, 1, 0, -1};

// Elem index is base 0
int get_element(char* signal, int signal_len, int elem_index) {
    int sum = 0;
    for (int i = 0; i < signal_len; i++) {
        int pattern_index = ((i+1) / (elem_index + 1));
        pattern_index %= 4;
        int multiplier = basepattern[pattern_index];
        int addition = (signal[i] - '0') * multiplier;
        // printf("%d*%d + ", multiplier, signal[i] - '0');
        sum += addition;
    }
    sum = sum < 0 ? -sum : sum;
    sum = sum % 10;
    // printf(" = %d\n", sum);
    return sum;
}

// operates on signal itself
void transform_signal(char* signal, int signal_len) {
    char copy[LINELEN];
    memcpy(copy, signal, signal_len);

    for (int i = 0; i < signal_len; i++) {
        signal[i] = get_element(copy, signal_len, i) + '0';
    }
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

    fgets(line, LINELEN, f);

    for (int i = 0; i < 100; i++) {
        transform_signal(line, strlen(line) - 1);
        printf("%s\n", line);
    }
    printf("Final transformed signal is %s\n", line);
    return 0;
}

