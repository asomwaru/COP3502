#include <stdio.h>
#include <stdlib.h>

int* read_nums(FILE* in, int len, int* array);

int main() {
    int nums;
    FILE* input = fopen("text.txt", "r");

    fscanf(input, "%d", &nums);
    int* arr = (int*)calloc(2*nums, sizeof(int));
    read_nums(input, nums*2, arr);

    for (int i=0; i<nums*2; i++) {
        printf("%d\n", arr[i]);
    }

    return 0;
}

int* read_nums(FILE* in, int len, int* array) {
    for (int i=0; i<len; i++) {
        fscanf(in, "%d", &array[i]);
    }

    return array;
}