#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int binary_search(int* arr, int size, int val);
void selection_sort(int* arr, int length);
void show_arr(int* arr, int size);

int main() {
    srand((unsigned)time(NULL));

    int size, val, index;

    printf("Enter the array size: ");
    scanf("%d", &size);

    int* arr = (int*)malloc(sizeof(int) * size);

    for (int i=0; i<size; i++) {
        if (rand() % 2 == 0)
            arr[i] = 1+rand() % 100;
        else
            arr[i] = (rand() % 100)-1;
    }

    printf("Unsorted: ");
    show_arr(arr, size);

    printf("Sorted: ");
    selection_sort(arr, size);
    show_arr(arr, size);

    printf("What value do you want to find? ");
    scanf("%d", &val);

    index = binary_search(arr, size, val);

    if (index == -1) {
        printf("Couldn't find your number.\n");

    } else {
        printf("Your number, %d, is at index %d.\n", val, index);
        printf("Arr: %d\n", arr[index]);

    }

    return 0;
}

void show_arr(int* arr, int size) {
    for (int i=0; i<size; i++) {
        printf("%d ", arr[i]);
    }

    printf("\n");
}

void selection_sort(int* arr, int length) {
    // i chose this algorithm because it was easiest to implement imo
    for (int i=0; i<length; i++) {
        int min = i;
        for (int j=i+1; j<length; j++) {
            if (arr[j] < arr[min]) {
                min = j;
            }
        }

        if (min != i) {
            int temp;

            temp = arr[min];
            arr[min] = arr[i];
            arr[i] = temp;
        }
    }
}


int binary_search(int* arr, int size, int val) {
    int low = 0, high = size-1;
    int mid;

    while (low <= high) {
        mid = (low+high)/ 2;

        if (arr[mid] == val) {
            return mid;

        } else if (val < arr[mid]) {
            high = mid-1;

        } else if (val > mid) {
            low = mid+1;
        }
    }

    return -1;
}