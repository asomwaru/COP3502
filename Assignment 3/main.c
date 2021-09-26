#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "leak_detector_c.h"


typedef struct monster { 
    int id;
    char name[64]; 
    char element[64]; 
    int population;
    double weight; 
} monster;

typedef struct {
    long long int compares;
    long long int copies;
} sort_results;


// required functions
int compareTo(monster* m1, monster*  m2,  int  criteria);
int isSorted(monster* m, int length, int criteria);

// helper functions
void swap(monster *m1, monster *m2, sort_results* res);
void sr_init(sort_results* res);
void out_to_csv(FILE* csv, sort_results res, float time_taken, int end_line);
 
// sorting wrappers
void selection_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out);
void bubble_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out);
void insertion_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out);
void quick_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out);
void merge_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out);
void merge_insert_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out);

// sorting algorithms
void selection_sort(monster* arr, int length, int criteria, sort_results* res);
void bubble_sort(monster* arr, int length, int criteria, sort_results* res);
void insertion_sort(monster* arr, int length, int criteria, sort_results* res);

int partition(monster* vals, int low, int high, int criteria, sort_results* res);
void quick_sort(monster* arr, int low, int high, int criteria, sort_results* res);

void merge(monster* arr, int left, int m, int right, int criteria, sort_results* res);
void merge_sort(monster* arr, int left, int right, int criteria, sort_results* res);

void merge_insert_sort(monster* arr, int left, int right, int criteria, sort_results* res);
void mod_insertion_sort(monster* arr, int left, int right, int criteria, sort_results* res);


int main() {
    atexit(report_mem_leak);
    srand((unsigned)time(NULL));

    FILE *out, *in; 
    char filename[20];

    out = fopen("out.txt", "w");

    for (int criteria=1; criteria<=3; criteria++){
        char csv_name[20];
        sprintf(csv_name, "crit_%d.csv", criteria);

        FILE* csv = fopen(csv_name, "w");

        fprintf(csv, "DataSize,SelectionSortCompare,SelectionSortCopy,SelectionSortTIme,BubbleSortCompare,BubbleSortCopy,BubbleSortTime,InsertionSortCompare,InsertionSortCopy,InsertionSortTime,MergeSortCompare,MergeSortCopy,MergeSortTime,Merge_InsertionSortCompare, Merge_InsertionSortCopy,Merge_InsertionSortTime,QuickSortCompare,QuickSortCopy,QuickSortTime\n");

        for (int multiplier=1; multiplier<=6; multiplier++) {
            sprintf(filename, "%d0k.txt", multiplier);
            int size = 10000 * multiplier;

            in = fopen(filename, "r");

            if (in == NULL) {
                break;
            }

            monster* m = (monster*)malloc(sizeof(monster) * size);
            for (int i=0; i<size; i++) {
                fscanf(in, "%s %s %d %lf", m[i].name, m[i].element, &m[i].population, &m[i].weight);
                m[i].id = 0;
            }

            float time_taken;
            sort_results res;
            sr_init(&res);

            fprintf(out, "========\n");
            fprintf(out, "Processing Criteria %d and file %s\n", criteria, filename);
            fprintf(out, "========\n");

            fprintf(csv, "%d,", size);

            selection_wrapper(m, size, criteria, &res, &time_taken, out);
            out_to_csv(csv, res, time_taken, 0);
            
            bubble_wrapper(m, size, criteria, &res, &time_taken, out);
            out_to_csv(csv, res, time_taken, 0);
            
            insertion_wrapper(m, size, criteria, &res, &time_taken, out);
            out_to_csv(csv, res, time_taken, 0);
            
            merge_wrapper(m, size, criteria, &res, &time_taken, out);
            out_to_csv(csv, res, time_taken, 0);
            
            merge_insert_wrapper(m, size, criteria, &res, &time_taken, out);
            out_to_csv(csv, res, time_taken, 0);
            
            quick_wrapper(m, size, criteria, &res, &time_taken, out);
            out_to_csv(csv, res, time_taken, 1);

            free(m);
        }

        fclose(csv);
    }

    fclose(in);
    fclose(out);
    return 0;
}


int compareTo(monster* m1, monster*  m2,  int  criteria) {
    /*

    Depending on the criterion, the function should return negative if m1 is smaller than  m2, 
    positive  if  m1  is  greater  than  m2,  
    and  0  if  both  are  same

    */

    if (criteria == 1) {
        return strcmp(m1->name, m2->name);

    } else if (criteria == 2) {
        if (m1->weight > m2->weight) {
            return 1;

        } else if (m1->weight < m2->weight) {
            return -1;

        } else {
            return 0;
        }
    } else {
        if (strcmp(m1->name, m2->name) == 0) {
            if (m1->weight > m2->weight) {
                return 1;

            } else if (m1->weight < m2->weight) {
                return -1;

            } else {
                return 0;
            }
        } else {
            return strcmp(m1->name, m2->name);
        }
    }
}

int isSorted(monster* m, int length, int criteria) {
    for (int i=0; i<length-1; i++) {
        if (compareTo(&m[i], &m[i+1], criteria) > 0) {
            printf("%d - %s : %s\n", i, m[i].name, m[i+1].name);
            return 0;
        }
    }

    return 1;
}


void selection_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out) {
    monster* cpy = (monster*)malloc(sizeof(monster) * length);
    memcpy(cpy, arr, sizeof(monster) * length);

    sr_init(res);

    fprintf(out, "Array status: not sorted by name before calling selection sort\n");
    fprintf(out, "Array status: sorted by name after calling selection sort\n");

    clock_t start_cpu, end_cpu; 
    start_cpu = clock();

    selection_sort(cpy, length, criteria, res);

    end_cpu = clock();
    *time = (((float)(end_cpu - start_cpu))  /  CLOCKS_PER_SEC);

    fprintf(out, "Total time taken %f second\n", *time);
    fprintf(out, "Total number of comparisons %lld\n", res->compares);
    fprintf(out, "Total number of copy operations %lld\n", res->copies);

    free(cpy);
}

void bubble_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out) {
    monster* cpy = (monster*)malloc(sizeof(monster) * length);
    memcpy(cpy, arr, sizeof(monster) * length);

    sr_init(res);

    fprintf(out, "Array status: not sorted by name before calling bubble sort\n");
    fprintf(out, "Array status: sorted by name after calling bubble sort\n");

    clock_t start_cpu, end_cpu; 
    start_cpu = clock();

    bubble_sort(cpy, length, criteria, res);

    end_cpu = clock();
    *time = (((float)(end_cpu - start_cpu))  /  CLOCKS_PER_SEC);

    fprintf(out, "Total time taken %f second\n", *time);
    fprintf(out, "Total number of comparisons %lld\n", res->compares);
    fprintf(out, "Total number of copy operations %lld\n", res->copies);

    free(cpy);
}

void insertion_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out) {
    monster* cpy = (monster*)malloc(sizeof(monster) * length);
    memcpy(cpy, arr, sizeof(monster) * length);

    sr_init(res);

    fprintf(out, "Array status: not sorted by name before calling insertion sort\n");
    fprintf(out, "Array status: sorted by name after calling insertion sort\n");

    clock_t start_cpu, end_cpu; 
    start_cpu = clock();

    insertion_sort(cpy, length, criteria, res);

    end_cpu = clock();
    *time = (((float)(end_cpu - start_cpu))  /  CLOCKS_PER_SEC);

    fprintf(out, "Total time taken %f second\n", *time);
    fprintf(out, "Total number of comparisons %lld\n", res->compares);
    fprintf(out, "Total number of copy operations %lld\n", res->copies);

    free(cpy);
}

void merge_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out) {
    monster* cpy = (monster*)malloc(sizeof(monster) * length);
    memcpy(cpy, arr, sizeof(monster) * length);

    sr_init(res);

    fprintf(out, "Array status: not sorted by name before calling merge sort\n");
    fprintf(out, "Array status: sorted by name after calling merge sort\n");

    clock_t start_cpu, end_cpu; 
    start_cpu = clock();

    merge_sort(cpy, 0, length-1, criteria, res);

    end_cpu = clock();
    *time = (((float)(end_cpu - start_cpu))  /  CLOCKS_PER_SEC);

    fprintf(out, "Total time taken %f second\n", *time);
    fprintf(out, "Total number of comparisons %lld\n", res->compares);
    fprintf(out, "Total number of copy operations %lld\n", res->copies);

    free(cpy);
}

void merge_insert_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out) {
    monster* cpy = (monster*)malloc(sizeof(monster) * length);
    memcpy(cpy, arr, sizeof(monster) * length);

    sr_init(res);

    fprintf(out, "Array status: not sorted by name before calling merge/insertion sort\n");
    fprintf(out, "Array status: sorted by name after calling merge/insertion sort\n");

    clock_t start_cpu, end_cpu; 
    start_cpu = clock();

    merge_insert_sort(cpy, 0, length-1, criteria, res);

    end_cpu = clock();
    *time = (((float)(end_cpu - start_cpu))  /  CLOCKS_PER_SEC);

    fprintf(out, "Total time taken %f second\n", *time);
    fprintf(out, "Total number of comparisons %lld\n", res->compares);
    fprintf(out, "Total number of copy operations %lld\n", res->copies);

    free(cpy);
}

void quick_wrapper(monster* arr, int length, int criteria, sort_results* res, float* time, FILE* out) {
    monster* cpy = (monster*)malloc(sizeof(monster) * length);
    memcpy(cpy, arr, sizeof(monster) * length);

    sr_init(res);

    fprintf(out, "Array status: not sorted by name before calling quick sort\n");
    fprintf(out, "Array status: sorted by name after calling quick sort\n");

    clock_t start_cpu, end_cpu; 
    start_cpu = clock();

    quick_sort(cpy, 0, length-1, criteria, res);

    end_cpu = clock();
    *time = (((float)(end_cpu - start_cpu))  /  CLOCKS_PER_SEC);

    fprintf(out, "Total time taken %f second\n", *time);
    fprintf(out, "Total number of comparisons %lld\n", res->compares);
    fprintf(out, "Total number of copy operations %lld\n", res->copies);

    free(cpy);
}


void selection_sort(monster* arr, int length, int criteria, sort_results* res) {
    for (int i=0; i<length-1; i++) {
        int min = i;

        for (int j=i+1; j<length; j++) {
            res->compares++;
            if (compareTo(&arr[j], &arr[min], criteria) < 0) {
                min = j;
            }
        }

        monster temp;

        temp = arr[min];
        arr[min] = arr[i];
        arr[i] = temp;
        
        res->copies += 3;
    }
}

void bubble_sort(monster* arr, int length, int criteria, sort_results* res) { 
    for (int i=0; i<length-1; i++) {
        for (int j=0; j<length-i-1; j++) {
            res->compares++;

            if (compareTo(&arr[j], &arr[j+1], criteria) > 0) {
                swap(&arr[j], &arr[j+1], res);
            }
        }
    }
}

void insertion_sort(monster* arr, int length, int criteria, sort_results* res) { 
    int j;
    monster item;

    for (int i = 1; i < length; i++) { 
        item = arr[i]; 
        res->copies++;       

        for(j=i-1; j>=0; j--) {  
            res->compares++;

            if (compareTo(&arr[j], &item, criteria) > 0) {
                arr[j+1] = arr[j];
                res->copies++;
            } else {

                break;
            }
        }
    
        arr[j+1] = item;
        res->copies++;
    }
}


int partition(monster* vals, int low, int high, int criteria, sort_results* res) {
    int i = low + rand()%(high-low+1);
    swap(&vals[low], &vals[i], res);

    int lowpos = low;
    low++; 

    while(low<=high) {
        while (low<=high && compareTo(&vals[low], &vals[lowpos], criteria) <= 0) {
            res->compares++;
            low++;
        }

        while (low<=high && compareTo(&vals[high], &vals[lowpos], criteria) > 0) {
            res->compares++;
            high--;
        }

        if (high > low) {
            swap(&vals[low], &vals[high], res);
        }
    }

    swap(&vals[lowpos], &vals[high], res);

    return high;
}

void quick_sort(monster* arr, int low, int high, int criteria, sort_results* res) {
    if (low < high) {
        int split = partition(arr, low, high, criteria, res);
        quick_sort(arr, low, split-1, criteria, res);
        quick_sort(arr, split+1, high, criteria, res);
    }
}


void merge(monster* arr, int left, int m, int right, int criteria, sort_results* res) {
    int i, j, k;
    int n1 = m - left + 1;
    int n2 = right - m;

    monster *L = (monster*) malloc(n1*sizeof(monster));
    monster *R = (monster*) malloc(n2*sizeof(monster));

    for (i = 0; i < n1; i++) {
        L[i] = arr[left + i];
        res->copies++;
    }

    for (j = 0; j < n2; j++) {
        R[j] = arr[m + 1 + j];
        res->copies++;
    }

    i = 0; 
    j = 0; 
    k = left;
    
    while (i < n1 && j < n2) {
        res->compares++;

        if (compareTo(&L[i], &R[j], criteria) <= 0) {
            arr[k] = L[i];

            res->copies++;
            i++;
        } else {
            arr[k] = R[j];

            res->copies++;
            j++;
        }

        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        res->copies++;
        i++;
        k++;
    }
    
    while (j < n2) {
        arr[k] = R[j];
        res->copies++;
        j++;
        k++;
    }

    free(L);
    free(R);
}

void merge_sort(monster* arr, int left, int right, int criteria, sort_results* res) {
    if (left < right) {
        int m = (left+right)/2;

        merge_sort(arr, left, m, criteria, res);
        merge_sort(arr, m+1, right, criteria, res);

        merge(arr, left, m, right, criteria, res);
    }
}


void merge_insert_sort(monster* arr, int left, int right, int criteria, sort_results* res) {
    if ((right - left + 1) <= 25) {
        mod_insertion_sort(arr, left, right, criteria, res);

    } else if (left < right) {
        int m = (left+right)/2;

        merge_insert_sort(arr, left, m, criteria, res);
        merge_insert_sort(arr, m+1, right, criteria, res);

        merge(arr, left, m, right, criteria, res);
    }
}

void mod_insertion_sort(monster* arr, int left, int right, int criteria, sort_results* res) {
    int j;
    monster item;

    for (int i = left+1; i <= right; i++) { 
        item = arr[i]; 
        res->copies++;       

        for(j=i-1; j>=left; j--) {  
            res->compares++;

            if (compareTo(&arr[j], &item, criteria) > 0) {
                arr[j+1] = arr[j];
                res->copies++;

            } else {
                break; 
            }
        }
    
        arr[j+1] = item;
        res->copies++;
    }
}


void swap(monster *m1, monster *m2, sort_results* res) {    
    monster temp = *m1;    
    *m1 = *m2;    
    *m2 = temp;

    res->copies += 3;
}

void sr_init(sort_results* res) {
    res->compares = 0;

    res->copies = 0;
}

void out_to_csv(FILE* csv, sort_results res, float time_taken, int end_line) {
    if (end_line) {
        fprintf(csv, "%lld,%lld,%f\n", res.compares, res.copies, time_taken);
    } else {
        fprintf(csv, "%lld,%lld,%f,", res.compares, res.copies, time_taken);
    }
}
