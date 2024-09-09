

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE (1024 * 1024 * 64) // 64 MB array
#define NUM_ACCESSES 100000000

int execute_t3() {
    int *array = (int*)malloc(ARRAY_SIZE * sizeof(int));
    long long int i, sum = 0;
    int offset = ARRAY_SIZE / 2;

    clock_t start = clock();

    for (i = 0; i < NUM_ACCESSES; i++) {
        sum += array[(i * offset) % ARRAY_SIZE];
    }

    clock_t end = clock();

    printf("Total Sum: %lld\n", sum);
    printf("Time taken: %lf seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    free(array);
    return 0;
}
