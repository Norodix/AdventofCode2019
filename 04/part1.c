#include <stdio.h>
#include <stdlib.h>

int rule1(int num)
{
    if (num < 100000) return 0;
    if (num > 999999) return 0;
    return 1;
}

int rule2(int num)
{
    for (int i = 1; i <= 10000; i*=10){
        if (((num / i) % 10) == ((num / (10*i)) % 10)) return 1;
    }
    return 0;
}

int rule3(int num)
{
    int min = 0;
    for (int i = 100000; i >= 1; i/=10){
        int test = num/i;
        test %= 10;
        if (test >= min){
            min = test;
        }
        else return 0;
    }
    return 1;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Must have 2 args, the start and end of the range\n");
        return -1;
    }
    int start = strtol(argv[1], NULL, 10);
    int end = strtol(argv[2], NULL, 10);
    int count = 0;

    for (int i = start; i <= end; i++) {
        if (rule1(i) && rule2(i) && rule3(i)){
            count++;
        }
    }
    printf("Total number of good values: %d\n", count);
}

