#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

uint8_t hw_mem[MEM_SIZE_B];
// 这六个函数的第一个 `xaddr_t`类型的参数都是待读写的内存地址，第二个`size_t`类型的参数（`len`）都是待读写的数据长度（字节数），对于虚拟地址读写来说，仅可以是1、2、4中的某一个值。对于所有的 `read()`函数，其返回的 `uint32_t`类型数据即为模拟内存中从 `addr`开始连续 `len`个字节的数据内容，若 `len`小于4，在返回值高位补0。对于所有的 `write()`函数，其第三个`uint32_t`类型的参数（`data`）即为要写入的数据，写入位置为模拟内存中从 `addr`开始连续 `len`个字节，若 `len`小于4，则将 `data`高位截断。注意NEMU采用小端方式存储数据。

#ifdef CACHE_TIMER
extern uint64_t cache_time;
#endif

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
#ifdef HAS_DEVICE_VGA
	if (is_mmio(paddr) != -1)
	{
		ret = mmio_read(paddr, len, is_mmio(paddr));
	}
	else
	{
#ifdef CACHE_ENABLED
		ret = cache_read(paddr, len);
#else
		ret = hw_mem_read(paddr, len);
#ifdef CACHE_TIMER
		cache_time += 10;	// cache miss penalty
#endif
#endif
	}
	
#else

#ifdef CACHE_ENABLED
	ret = cache_read(paddr, len);
#else
	ret = hw_mem_read(paddr, len);
#ifdef CACHE_TIMER
	cache_time += 10;	// cache miss penalty
#endif
#endif
#endif

	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
#ifdef HAS_DEVICE_VGA
	if (is_mmio(paddr) != -1)
	{
		mmio_write(paddr, len, data, is_mmio(paddr));
	}
	else
	{
#ifdef CACHE_ENABLED
	cache_write(paddr, len, data);
#else
	hw_mem_write(paddr, len, data);
#endif
	}	

#else
#ifdef CACHE_ENABLED
	cache_write(paddr, len, data);
#else
	hw_mem_write(paddr, len, data);
#endif
#endif
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{
#ifdef IA32_PAGE
	assert(len == 1 || len == 2 || len == 4);
	if(cpu.cr0.pg == 1 && cpu.cr0.pe == 1){
		// 跨页访问，高20位不同
		if( laddr >> 12 != (laddr + len - 1) >> 12 ){
			// 读取的数据跨页
			uint32_t data = 0;
			uint32_t len1 = (((laddr >> 12) + 1) << 12) - laddr;
			uint32_t len2 = len - len1;
			paddr_t paddr1 = page_translate(laddr);
			paddr_t paddr2 = page_translate(laddr + len1);
			data = paddr_read(paddr1, len1);
			data |= paddr_read(paddr2, len2) << (len1 << 3);
			return data;
		}
		else{
			// 使用paddr应该比课件中的hwaddr要好，因为还可以引入cache
			paddr_t paddr = page_translate(laddr);
			return paddr_read(paddr, len);
		}
	}
	// 实模式下
	else{
		return paddr_read(laddr, len);
	}
#else
	return paddr_read(laddr, len);
#endif
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
#ifdef IA32_PAGE
	assert(len == 1 || len == 2 || len == 4);
	if(cpu.cr0.pg == 1 && cpu.cr0.pe == 1){
		// 跨页访问，高20位不同
		if( laddr >> 12 != (laddr + len - 1) >> 12 ){
			// 写入的数据跨页
			uint32_t len1 = (((laddr >> 12) + 1) << 12) - laddr;
			uint32_t len2 = len - len1;
			paddr_t paddr1 = page_translate(laddr);
			paddr_t paddr2 = page_translate(laddr + len1);
			paddr_write(paddr1, len1, data);
			paddr_write(paddr2, len2, data >> (len1 << 3));
		}
		else{
			paddr_t paddr = page_translate(laddr);
			paddr_write(paddr, len, data);
		}
	}
	// 实模式下
	else{
		paddr_write(laddr, len, data);
	}
#else
	paddr_write(laddr, len, data);
#endif
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
#ifdef IA32_SEG
	// 实模式下
	if (cpu.cr0.pe == 0)
	{
		return laddr_read(vaddr, len);
	}
	// 保护模式下
	else{
		laddr_t laddr = segment_translate(vaddr, sreg);
		return laddr_read(laddr, len);
	}
#else
	return laddr_read(vaddr, len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
#ifdef IA32_SEG
	// 实模式下
	if (cpu.cr0.pe == 0)
	{
		laddr_write(vaddr, len, data);
	}
	// 保护模式下
	else{
		laddr_t laddr = segment_translate(vaddr, sreg);
		laddr_write(laddr, len, data);
	}
#else
	laddr_write(vaddr, len, data);
#endif
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);

#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif

#ifdef CACHE_ENABLED
	init_cache();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}

// 约定内存从物理地址 `0xa0000` 开始，长度为 `320 * 200` 字节的区间为显存区间
// 相当于这一块物理地址实际不是内存，而是io，此处io设备是显存，然后将此处io映射到虚拟地址中去
 
// mmio部分已经在nemu\src\device\io\mm_io.c中被布置好了
// 重点是修改vaddr_read和vaddr_write函数，使得在访问显存区间时，实际上是访问显存io设备