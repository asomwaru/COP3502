#include <stdio.h>
#include <stdlib.h>


int power(int base, int exp);
int crazysum(int n);
int weightedSum(int numbers[], int n);


int main() {
    int* arr;
    int size;

    printf("Enter size: ");
    scanf("%d", &size);

    arr = (int*)malloc(sizeof(int) * size);
    for (int i=0; i<size; i++) {
        printf("Index #%d: ", i);
        scanf("%d", &arr[i]);
    }

    printf("Weighted: %d\n", weightedSum(arr, size));
    return 0;
}

int power(int base, int exp) {
    int total = base;

    for (int i=1; i<exp; i++) {
        total = total*base;
    }

    return total;
}

int crazysum(int n) {
    if (n == 1) {
        return 1;
    } else {
        return power(n, n) + crazysum(n-1);
    }
}

int zeros(int n) {
    if (n == 0)
        return 0;
    
    return n/5 + zeros(n/5);
}

int weightedSum(int numbers[], int n) {
    if (n == 0) {
        return 0;
    } else {
        return numbers[n-1]*(n) + weightedSum(numbers, n-1);
    }
}