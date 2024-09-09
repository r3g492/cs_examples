#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define ARRAY_SIZE (1024 * 1024 * 16)

#define L1_SIZE 2
#define L2_SIZE 4
#define L3_SIZE 8

#define L1_ACCESS_TIME 1
#define L2_ACCESS_TIME 10
#define L3_ACCESS_TIME 50
#define MEMORY_ACCESS_TIME 200

#define CACHE_SIZE 8


typedef struct {
    int valid;
    long address;
    int data;
} CacheLine;

typedef struct {
    int accessTime;
    CacheLine cacheMap[L1_SIZE];
} L1Cache;

typedef struct {
    int accessTime;
    CacheLine cacheMap[L2_SIZE];
} L2Cache;

typedef struct {
    int accessTime;
    CacheLine cacheMap[L3_SIZE];
} L3Cache;

int hash_function(long address, int cache_size) {
    return address % cache_size;
}

int find_in_cache(CacheLine *cache, int cache_size, long address) {
    int index = hash_function(address, cache_size);
    for (int i = 0; i < cache_size; i++) {
        int probe_index = (index + i) % cache_size;
        if (cache[probe_index].valid && cache[probe_index].address == address) {
            return cache[probe_index].data;
        } else if (!cache[probe_index].valid) {
            break;
        }
    }
    return -1;
}

void load_into_cache(CacheLine *cache, int cache_size, long address, int data) {
    int index = hash_function(address, cache_size);
    for (int i = 0; i < cache_size; i++) {
        int probe_index = (index + i) % cache_size;
        if (!cache[probe_index].valid || cache[probe_index].address == address) {
            cache[probe_index].valid = 1;
            cache[probe_index].address = address;
            cache[probe_index].data = data;
            return;
        }
    }
}

int execute_t3() {
//    CacheLine cache[CACHE_SIZE] = {0};
//
//    load_into_cache(cache, CACHE_SIZE, 123456, 10);
//    load_into_cache(cache, CACHE_SIZE, 654321, 20);
//
//    // Simulate accessing the cache
//    int data = find_in_cache(cache, CACHE_SIZE, 123456);
//    printf("Data for address 123456: %d\n", data);
//
//    data = find_in_cache(cache, CACHE_SIZE, 654321);
//    printf("Data for address 654321: %d\n", data);
//
//    data = find_in_cache(cache, CACHE_SIZE, 111111);
//    if (data == -1) {
//        printf("Cache miss for address 111111\n");
//    }

    CacheLine l1CacheMap[L1_SIZE] = {0};
    L1Cache l1Cache = {L1_ACCESS_TIME, {0}};

    CacheLine l2CacheMap[L2_SIZE] = {0};
    L2Cache l2Cache = {L2_ACCESS_TIME, {0}};

    CacheLine l3CacheMap[L3_SIZE] = {0};
    L2Cache l3Cache = {L3_ACCESS_TIME, {0}};

    return 0;
}
