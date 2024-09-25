#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ARRAY_SIZE (1024 * 1024 * 16)

#define L1_SIZE 1
#define L2_SIZE 2
#define L3_SIZE 4

#define L1_ACCESS_TIME 1
#define L2_ACCESS_TIME 10
#define L3_ACCESS_TIME 50
#define MEMORY_ACCESS_TIME 200

typedef struct {
    int valid;
    long address;
    int data;
} CacheLine;

typedef struct {
    int accessTime;
    CacheLine *cacheMap;
    int size;
} Cache;

char result[256];

int hash_function(long address, int cache_size) {
    return address % cache_size;
}

int find_in_cache(Cache *cache, long address) {
    int index = hash_function(address, cache->size);
    for (int i = 0; i < cache->size; i++) {
        int probe_index = (index + i) % cache->size;
        if (cache->cacheMap[probe_index].valid && cache->cacheMap[probe_index].address == address) {
            return cache->cacheMap[probe_index].data;
        }
        if (!cache->cacheMap[probe_index].valid) {
            break;
        }
    }
    return -1;
}

void load_into_cache(Cache *cache, long address, int data) {
    int index = hash_function(address, cache->size);
    for (int i = 0; i < cache->size; i++) {
        int probe_index = (index + i) % cache->size;
        if (!cache->cacheMap[probe_index].valid || cache->cacheMap[probe_index].address == address) {
            cache->cacheMap[probe_index].valid = 1;
            cache->cacheMap[probe_index].address = address;
            cache->cacheMap[probe_index].data = data;
            return;
        }
    }
}

void print_cache(Cache *cache, const char *cache_name) {
    printf("%s contents:\n", cache_name);
    for (int i = 0; i < cache->size; i++) {
        if (cache->cacheMap[i].valid) {
            printf("  slot %d: address = 0x%lX, Data = %d\n", i, cache->cacheMap[i].address, cache->cacheMap[i].data);
        } else {
            printf("  slot %d: invalid\n", i);
        }
    }
}

void cache_logic(Cache *l1Cache, Cache *l2Cache, Cache *l3Cache) {
    printf("\nsynchronizing caches...\n");
}

int access_data(Cache *l1Cache, Cache *l2Cache, Cache *l3Cache, long address) {
    int data;

    memset(result, 0, sizeof(result));

    data = find_in_cache(l1Cache, address);
    if (data != -1) {
        printf("l1 cache hit\n");
        snprintf(result, sizeof(result), "l1 cache hit. accessed address: 0x%lX", address);
        return l1Cache->accessTime;
    }

    data = find_in_cache(l2Cache, address);
    if (data != -1) {
        printf("l2 cache hit\n");
        snprintf(result, sizeof(result), "l2 cache hit. accessed address: 0x%lX", address);
        load_into_cache(l1Cache, address, data);
        return l2Cache->accessTime;
    }

    data = find_in_cache(l3Cache, address);
    if (data != -1) {
        printf("l3 cache hit\n");
        snprintf(result, sizeof(result), "l3 cache hit. accessed address: 0x%lX", address);
        load_into_cache(l2Cache, address, data);
        load_into_cache(l1Cache, address, data);
        return l3Cache->accessTime;
    }

    printf("cache miss, loading from memory\n");
    snprintf(result, sizeof(result), "cache miss. accessed address: 0x%lX", address);
    data = rand();
    load_into_cache(l3Cache, address, data);
    load_into_cache(l2Cache, address, data);
    load_into_cache(l1Cache, address, data);
    return MEMORY_ACCESS_TIME;
}

int access_data_only_l3(Cache *l3Cache, long address) {
    int data;

    data = find_in_cache(l3Cache, address);
    if (data != -1) {
        printf("L3 cache hit\n");
        return l3Cache->accessTime;
    }

    printf("Cache miss, loading from memory\n");
    data = rand();
    load_into_cache(l3Cache, address, data);
    return MEMORY_ACCESS_TIME;
}


int execute_t3() {
    srand(time(NULL));

    Cache l1Cache = {L1_ACCESS_TIME, malloc(L1_SIZE * sizeof(CacheLine)), L1_SIZE};
    Cache l2Cache = {L2_ACCESS_TIME, malloc(L2_SIZE * sizeof(CacheLine)), L2_SIZE};
    Cache l3Cache = {L3_ACCESS_TIME, malloc(L3_SIZE * sizeof(CacheLine)), L3_SIZE};

    for (int i = 0; i < L1_SIZE; i++) l1Cache.cacheMap[i].valid = 0;
    for (int i = 0; i < L2_SIZE; i++) l2Cache.cacheMap[i].valid = 0;
    for (int i = 0; i < L3_SIZE; i++) l3Cache.cacheMap[i].valid = 0;
    // no cache hit
    // long addresses[] = {0x1A2B3C, 0x1A2B4C, 0x1A2B5C, 0x1A2B6C, 0x1A2B7C};
    long addresses[] = {0x1A2B3C, 0x1A2B3C, 0x3F4E5D, 0x7A9B8C, 0x7A9B8C};

    int totalTime = 0;
    int times = 5;
    for (int i = 0; i < times; i++) {
        printf("Accessing address 0x%lX...\n", addresses[i]);

        int timeTaken = access_data(&l1Cache, &l2Cache, &l3Cache, addresses[i]);
        // int timeTaken = access_data_only_l3(&l3Cache, addresses[i]);
        printf("Time taken: %d cycles\n", timeTaken);
        totalTime += timeTaken;

        print_cache(&l1Cache, "L1 Cache");
        print_cache(&l2Cache, "L2 Cache");
        print_cache(&l3Cache, "L3 Cache");

        cache_logic(&l1Cache, &l2Cache, &l3Cache);

        printf("\n");
    }

    int average = totalTime / times;

    printf("\nTotal access time: %d cycles\n", totalTime);
    printf("Average: %d cycles\n", average);

    free(l1Cache.cacheMap);
    free(l2Cache.cacheMap);
    free(l3Cache.cacheMap);

    return 0;
}


int change_data(Cache *cache, long address, int newData) {
    int index = hash_function(address, cache->size);
    for (int i = 0; i < cache->size; i++) {
        int probe_index = (index + i) % cache->size;
        if (cache->cacheMap[probe_index].valid && cache->cacheMap[probe_index].address == address) {
            cache->cacheMap[probe_index].data = newData;
            return newData;
        }
        if (!cache->cacheMap[probe_index].valid) {
            break;
        }
    }
    return -1;
}

void clear_console() {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For Linux/Unix/OSX
#endif
}

int execute_t3_real_time() {
    srand(time(NULL));

    Cache l1Cache = {L1_ACCESS_TIME, malloc(L1_SIZE * sizeof(CacheLine)), L1_SIZE};
    Cache l2Cache = {L2_ACCESS_TIME, malloc(L2_SIZE * sizeof(CacheLine)), L2_SIZE};
    Cache l3Cache = {L3_ACCESS_TIME, malloc(L3_SIZE * sizeof(CacheLine)), L3_SIZE};

    for (int i = 0; i < L1_SIZE; i++) l1Cache.cacheMap[i].valid = 0;
    for (int i = 0; i < L2_SIZE; i++) l2Cache.cacheMap[i].valid = 0;
    for (int i = 0; i < L3_SIZE; i++) l3Cache.cacheMap[i].valid = 0;

    int totalTime = 0;
    int times = 0;
    char input[256];
    long address;
    int newData;
    char cacheName[4];

    while (1) {
        clear_console();

        printf("cache status: \n");
        print_cache(&l1Cache, "l1 cache");
        print_cache(&l2Cache, "l2 cache");
        print_cache(&l3Cache, "l3 cache");
        printf("\n");

        printf("%s", result);
        printf("\naccess times: %d cycles\n", times);
        printf("\ntotal access time: %d cycles\n", totalTime);
        if (times != 0) {
            int average = totalTime / times;
            printf("average: %d cycles\n", average);
        }
        printf("enter address (in hex) or type 'exit' to quit: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            printf("exiting program.\n");
            break;
        }

        // Handle data change input like "l1 1A2B3C 12543"
        if (sscanf(input, "%s %lx %d", cacheName, &address, &newData) == 3) {
            printf("changing data in %s cache at address 0x%lX to %d\n", cacheName, address, newData);
            if (strcmp(cacheName, "l1") == 0) {
                if (change_data(&l1Cache, address, newData) == -1) {
                    printf("Address not found in L1 cache.\n");
                } else {
                    printf("Address 0x%lX in L1 cache changed to %d\n", address, newData);
                }
            } else if (strcmp(cacheName, "l2") == 0) {
                if (change_data(&l2Cache, address, newData) == -1) {
                    printf("Address not found in L2 cache.\n");
                } else {
                    printf("Address 0x%lX in L2 cache changed to %d\n", address, newData);
                }
            } else if (strcmp(cacheName, "l3") == 0) {
                if (change_data(&l3Cache, address, newData) == -1) {
                    printf("address not found in L3 cache.\n");
                } else {
                    printf("address 0x%lX in L3 cache changed to %d\n", address, newData);
                }
            } else {
                printf("invalid cache name. Please use 'l1', 'l2', or 'l3'.\n");
            }
        }
        // Handle address lookup
        else if (sscanf(input, "%lx", &address) == 1) {
            times++;
            printf("address entered: 0x%lX\n", address);
            int timeTaken = access_data(&l1Cache, &l2Cache, &l3Cache, address);
            totalTime += timeTaken;

            cache_logic(&l1Cache, &l2Cache, &l3Cache);

            printf("\n");
        } else {
            printf("invalid input, please enter a valid hex address or 'exit'.\n");
        }
        usleep(10000);
    }

    free(l1Cache.cacheMap);
    free(l2Cache.cacheMap);
    free(l3Cache.cacheMap);

    return 0;
}

int execute_t3_real_time_two_cores() {
    srand(time(NULL));

    Cache l1Cache = {L1_ACCESS_TIME, malloc(L1_SIZE * sizeof(CacheLine)), L1_SIZE};
    Cache l2Cache = {L2_ACCESS_TIME, malloc(L2_SIZE * sizeof(CacheLine)), L2_SIZE};
    Cache l3Cache = {L3_ACCESS_TIME, malloc(L3_SIZE * sizeof(CacheLine)), L3_SIZE};

    for (int i = 0; i < L1_SIZE; i++) l1Cache.cacheMap[i].valid = 0;
    for (int i = 0; i < L2_SIZE; i++) l2Cache.cacheMap[i].valid = 0;
    for (int i = 0; i < L3_SIZE; i++) l3Cache.cacheMap[i].valid = 0;

    Cache l1Cache2 = {L1_ACCESS_TIME, malloc(L1_SIZE * sizeof(CacheLine)), L1_SIZE};
    Cache l2Cache2 = {L2_ACCESS_TIME, malloc(L2_SIZE * sizeof(CacheLine)), L2_SIZE};
    Cache l3Cache2 = {L3_ACCESS_TIME, malloc(L3_SIZE * sizeof(CacheLine)), L3_SIZE};

    for (int i = 0; i < L1_SIZE; i++) l1Cache2.cacheMap[i].valid = 0;
    for (int i = 0; i < L2_SIZE; i++) l2Cache2.cacheMap[i].valid = 0;
    for (int i = 0; i < L3_SIZE; i++) l3Cache2.cacheMap[i].valid = 0;

    int totalTime = 0;
    int times = 0;

    int totalTime2 = 0;
    int times2 = 0;

    char input[256];
    long address;
    int newData;
    char coreName[6];
    char cacheName[4];

    while (1) {
        clear_console();

        printf("cache status (Core 1): \n");
        print_cache(&l1Cache, "l1 cache");
        print_cache(&l2Cache, "l2 cache");
        print_cache(&l3Cache, "l3 cache");

        printf("\ncache status (Core 2): \n");
        print_cache(&l1Cache2, "l1 cache");
        print_cache(&l2Cache2, "l2 cache");
        print_cache(&l3Cache2, "l3 cache");

        printf("\n%s", result);
        printf("\ncore 1 - access times: %d cycles\n", times);
        printf("core 1 - total access time: %d cycles\n", totalTime);
        if (times != 0) {
            int average = totalTime / times;
            printf("core 1 - average: %d cycles\n", average);
        }
        printf("core 2 - access times: %d cycles\n", times2);
        printf("core 2 - total access time: %d cycles\n", totalTime2);
        if (times2 != 0) {
            int average2 = totalTime2 / times2;
            printf("core 2 - average: %d cycles\n", average2);
        }

        printf("\nenter core (core1/core2), cache (l1/l2/l3), address (in hex), or type 'exit' to quit: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Remove newline

        if (strcmp(input, "exit") == 0) {
            printf("exiting program.\n");
            break;
        }

        if (sscanf(input, "%s %s %lx %d", coreName, cacheName, &address, &newData) == 4) {
            printf("changing data in %s %s cache at address 0x%lX to %d\n", coreName, cacheName, address, newData);

            Cache *cacheToModify = NULL;
            if (strcmp(coreName, "core1") == 0) {
                if (strcmp(cacheName, "l1") == 0) cacheToModify = &l1Cache;
                else if (strcmp(cacheName, "l2") == 0) cacheToModify = &l2Cache;
                else if (strcmp(cacheName, "l3") == 0) cacheToModify = &l3Cache;
            } else if (strcmp(coreName, "core2") == 0) {
                if (strcmp(cacheName, "l1") == 0) cacheToModify = &l1Cache2;
                else if (strcmp(cacheName, "l2") == 0) cacheToModify = &l2Cache2;
                else if (strcmp(cacheName, "l3") == 0) cacheToModify = &l3Cache2;
            }

            if (cacheToModify) {
                if (change_data(cacheToModify, address, newData) == -1) {
                    printf("address not found in %s cache.\n", cacheName);
                } else {
                    printf("address 0x%lX in %s cache changed to %d\n", address, cacheName, newData);
                }
            } else {
                printf("invalid cache or core name. please use 'core1' or 'core2' and 'l1', 'l2', or 'l3'.\n");
            }
        }
        else if (sscanf(input, "%s %lx", coreName, &address) == 2) {
            printf("accessing data in %s's cache at address 0x%lX\n", coreName, address);

            int *coreTime = NULL;
            int *coreTimes = NULL;

            if (strcmp(coreName, "core1") == 0) {
                coreTime = &totalTime;
                coreTimes = &times;

                (*coreTimes)++;
                int timeTaken = access_data(&l1Cache, &l2Cache, &l3Cache, address);
                *coreTime += timeTaken;
                cache_logic(&l1Cache, &l2Cache, &l3Cache);
                printf("time taken: %d cycles\n", timeTaken);
            } else if (strcmp(coreName, "core2") == 0) {
                coreTime = &totalTime2;
                coreTimes = &times2;

                (*coreTimes)++;
                int timeTaken = access_data(&l1Cache2, &l2Cache2, &l3Cache2, address);
                *coreTime += timeTaken;
                cache_logic(&l1Cache2, &l2Cache2, &l3Cache2);
                printf("time taken: %d cycles\n", timeTaken);
            } else {
                printf("invalid core name. Please use 'core1' or 'core2'.\n");
            }
        }
        else {
            printf("invalid input, please enter core, cache, and address in hex, or 'exit' to quit.\n");
        }

        usleep(10000);
    }

    free(l1Cache.cacheMap);
    free(l2Cache.cacheMap);
    free(l3Cache.cacheMap);
    free(l1Cache2.cacheMap);
    free(l2Cache2.cacheMap);
    free(l3Cache2.cacheMap);

    return 0;
}
