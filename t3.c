#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE (1024 * 1024 * 16)

#define L1_SIZE 2
#define L2_SIZE 4
#define L3_SIZE 8

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
        } else if (!cache->cacheMap[probe_index].valid) {
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

int execute_t3() {
    srand(time(NULL));

    Cache l1Cache = {L1_ACCESS_TIME, malloc(L1_SIZE * sizeof(CacheLine)), L1_SIZE};
    Cache l2Cache = {L2_ACCESS_TIME, malloc(L2_SIZE * sizeof(CacheLine)), L2_SIZE};
    Cache l3Cache = {L3_ACCESS_TIME, malloc(L3_SIZE * sizeof(CacheLine)), L3_SIZE};

    for (int i = 0; i < L1_SIZE; i++) l1Cache.cacheMap[i].valid = 0;
    for (int i = 0; i < L2_SIZE; i++) l2Cache.cacheMap[i].valid = 0;
    for (int i = 0; i < L3_SIZE; i++) l3Cache.cacheMap[i].valid = 0;

    long addresses[] = {0x1A2B3C, 0x1A2B3C, 0x3F4E5D, 0x7A9B8C, 0x1A2B3C};
    int totalTime = 0;

    for (int i = 0; i < 5; i++) {
        printf("Accessing address 0x%lX...\n", addresses[i]);
        int timeTaken = access_data(&l1Cache, &l2Cache, &l3Cache, addresses[i]);
        printf("Time taken: %d cycles\n\n", timeTaken);
        totalTime += timeTaken;
    }

    printf("Total access time: %d cycles\n", totalTime);

    free(l1Cache.cacheMap);
    free(l2Cache.cacheMap);
    free(l3Cache.cacheMap);

    return 0;
}
