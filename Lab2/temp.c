#include <stdio.h>
#include <stdlib.h>
#include "leak_detector_c.h"

int main() {
    atexit(report_mem_leak);
    int* arr = (int*)malloc(sizeof(int)*10);
    char* str = (char*)malloc(sizeof(char)*51);

    return 0;
}