#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("%d\n", strcmp("cat", "dog"));
    printf("%d\n", strcmp("cat", "bear"));
    printf("%d\n", strcmp("dog", "deer"));
    printf("%d\n", strcmp("dog", "tiger"));

    return 0;
}