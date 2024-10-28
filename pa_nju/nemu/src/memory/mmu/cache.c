#include "memory/mmu/cache.h"
#include <stdlib.h>

extern uint32_t hw_mem_read(paddr_t paddr, size_t len);
extern void hw_mem_write(paddr_t paddr, size_t len, uint32_t data);

#ifdef CACHE_TIMER
uint64_t cache_time = 0;
uint64_t cache_hit = 0;
uint64_t cache_miss = 0;
#endif

#ifdef CACHE_ENABLED
// 全局变量cache
CacheLine cache[CACHE_SIZE_LINE];


// init the cache
void init_cache()
{
	// implement me in PA 3-1
	// clear all the valid bits
	for (int i = 0; i < CACHE_SIZE_LINE; i++)
	{
		cache[i].valid = false;
	}
#ifdef CACHE_TIMER
	cache_time = 0;
	cache_hit = 0;
	cache_miss = 0;
#endif
	return;
}

// check and return the index(0-CACHE_GROUP_SIZE-1) of the block in the group of cache
// if hit, return the index of the block
// if miss, return -1
int8_t check_cache(paddr_t paddr)
{
	// implement me in PA 3-1
	uint32_t tag = paddr >> (32 - TAG_LEN);
	uint32_t group = (paddr << TAG_LEN) >> (32 - GROUP_LEN);

	uint32_t index_cache = group * CACHE_GROUP_SIZE;
	for (int i = 0; i < CACHE_GROUP_SIZE; i++)
	{
		if (cache[index_cache + i].valid && cache[index_cache + i].tag == tag)
		{
			return i;
		}
	}
	return -1;
}

// find an empty line in the group of cache
// if find, return the index of the empty line
// if not find, return -1
int8_t find_empty_line(paddr_t paddr)
{
	// implement me in PA 3-1
	uint32_t group = (paddr << TAG_LEN) >> (32 - GROUP_LEN);

	uint32_t index_cache = group * CACHE_GROUP_SIZE;
	for (int i = 0; i < CACHE_GROUP_SIZE; i++)
	{
		if (!cache[index_cache + i].valid)
		{
			return i;
		}
	}
	return -1;
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// 返回值最多为4字节，规范len长度
	if (len<0 || len>4)
	{
		printf("Error: wrong len\n");
		return 0;
	}
	if (len == 0)
	{
		return 0;
	}

	// implement me in PA 3-1
	// 从虚拟地址解析出TAG_LEN位的tag，GROUP_LEN位的group，OFFSET_LEN位的offset
	uint32_t tag = paddr >> (32 - TAG_LEN);
	uint32_t group = (paddr << TAG_LEN) >> (32 - GROUP_LEN);
	uint32_t offset = (paddr << (TAG_LEN + GROUP_LEN)) >> (32 - OFFSET_LEN);
	uint32_t ret = 0;

	// 检查是否跨块，跨块则拆分后递归调用
	if(offset + len - 1 >= CACHE_LINE_SIZE)
	{
		// 低地址部分
		ret = cache_read(paddr, CACHE_LINE_SIZE - offset);
		// 高地址部分
		ret = ret | (cache_read(paddr + CACHE_LINE_SIZE - offset, len - (CACHE_LINE_SIZE - offset)) << ((CACHE_LINE_SIZE - offset) * 8));
		// CACHE_LINE_SIZE - offset 是低地址部分的长度
		return ret;
	}
	else
	{

		// 在组内查找
		int8_t index = check_cache(paddr);
		// hit
		if (index != -1)
		{
#ifdef CACHE_TIMER
			cache_time += 1;
			cache_hit += 1;
#endif
			// 从cache中读取数据
			memcpy(&ret, cache[group * CACHE_GROUP_SIZE + index].data + offset, len);

			return ret;
		}
		// miss
		else
		{
#ifdef CACHE_TIMER
			cache_time += 10;
			cache_miss += 1;
#endif
			// 从内存中读取数据
			// 不能用paddr_read，因为paddr_read会调用cache_read！
			ret = hw_mem_read(paddr, len);
			// 将对应的整个块数据写入cache
			index = find_empty_line(paddr);
			if (index != -1)
			{
				// 有空行
				cache[group * CACHE_GROUP_SIZE + index].valid = true;
				cache[group * CACHE_GROUP_SIZE + index].tag = tag;
				// 将整个块数据写入cache
				// 从内存中读数据不能直接用memcpy，因为paddr不是本机对应地址，要用hw_mem_read
				// hw_mem_read一次最多读4字节，所以要循环读取
				for(int i = 0; i < CACHE_LINE_SIZE; i=i+4)
				{
					uint32_t result = hw_mem_read((paddr >> OFFSET_LEN << OFFSET_LEN) + i, 4);
					memcpy(cache[group * CACHE_GROUP_SIZE + index].data + i, &result, 4);
				}

			}
			else
			{
				// 没有空行，随机替换
				index = (uint8_t)rand() % CACHE_GROUP_SIZE;
				cache[group * CACHE_GROUP_SIZE + index].valid = true;
				cache[group * CACHE_GROUP_SIZE + index].tag = tag;
				// 将整个块数据写入cache
				for(int i = 0; i < CACHE_LINE_SIZE; i=i+4)
				{
					uint32_t result = hw_mem_read((paddr >> OFFSET_LEN << OFFSET_LEN) + i, 4);
					memcpy(cache[group * CACHE_GROUP_SIZE + index].data + i, &result, 4);
				}
			}
			return ret;
		}
	}

	return 0;
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	// implement me in PA 3-1
	// 从虚拟地址解析出TAG_LEN位的tag，GROUP_LEN位的group，OFFSET_LEN位的offset
	// uint32_t tag = paddr >> (32 - TAG_LEN); // unused
	uint32_t group = (paddr << TAG_LEN) >> (32 - GROUP_LEN);
	uint32_t offset = (paddr << (TAG_LEN + GROUP_LEN)) >> (32 - OFFSET_LEN);

	// 检查是否跨块，跨块则拆分后递归调用
	if(offset + len - 1 >= CACHE_LINE_SIZE)
	{
		// 低地址部分
		cache_write(paddr, CACHE_LINE_SIZE - offset, data & ((1 << ((CACHE_LINE_SIZE - offset) * 8)) - 1));
		// 高地址部分
		cache_write(paddr + CACHE_LINE_SIZE - offset, len - (CACHE_LINE_SIZE - offset), data >> ((CACHE_LINE_SIZE - offset) * 8));
		return;
	}
	else
	{
		// 在组内查找
		int8_t index = check_cache(paddr);
		// hit
		if (index != -1)
		{
			// 写入cache
			memcpy(cache[group * CACHE_GROUP_SIZE + index].data + offset, &data, len);
			// 写入内存
			hw_mem_write(paddr, len, data);
			return;
		}
		// miss
		else
		{
			// not write allocate
			// 直接写入内存
			hw_mem_write(paddr, len, data);
			return;
		}
	}
	
}

#endif
