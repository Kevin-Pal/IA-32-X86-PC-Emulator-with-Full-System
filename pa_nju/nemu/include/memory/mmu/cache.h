#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"

#ifdef CACHE_ENABLED

// 1024行，每行64B，总共64KB
// 128组，每组8行
#define CACHE_SIZE_B 64 * 1024
#define CACHE_SIZE_LINE 1024
#define CACHE_SIZE_GROUP 128
#define CACHE_LINE_SIZE 64
#define CACHE_GROUP_SIZE 8

#define TAG_LEN 19
#define GROUP_LEN 7
#define OFFSET_LEN 6

// define the structure of cache
typedef struct {
    bool valid; // 理论1bit 
    uint32_t tag;   // 理论19bit
    uint8_t data[CACHE_LINE_SIZE];  // 64B
} CacheLine;

// init the cache
void init_cache();

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data);

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len);

#endif

#endif
