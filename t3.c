#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
            printf("  Slot %d: Address = 0x%lX, Data = %d\n", i, cache->cacheMap[i].address, cache->cacheMap[i].data);
        } else {
            printf("  Slot %d: Invalid\n", i);
        }
    }
}

void cache_logic(Cache *l1Cache, Cache *l2Cache, Cache *l3Cache) {
    printf("\nSynchronizing caches...\n");
}

int access_data(Cache *l1Cache, Cache *l2Cache, Cache *l3Cache, long address) {
    int data;

    data = find_in_cache(l1Cache, address);
    if (data != -1) {
        printf("L1 cache hit\n");
        return l1Cache->accessTime;
    }

    data = find_in_cache(l2Cache, address);
    if (data != -1) {
        printf("L2 cache hit\n");
        load_into_cache(l1Cache, address, data);
        return l2Cache->accessTime;
    }

    data = find_in_cache(l3Cache, address);
    if (data != -1) {
        printf("L3 cache hit\n");
        load_into_cache(l2Cache, address, data);
        load_into_cache(l1Cache, address, data);
        return l3Cache->accessTime;
    }

    printf("Cache miss, loading from memory\n");
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