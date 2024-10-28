#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	// printf("\nPlease implement page_translate()\n");
	// fflush(stdout);
	// assert(0);

	// 页目录基址
	uint32_t dir = laddr >> 22;
	// 页表基址
	uint32_t page = (laddr >> 12) & 0x3ff;
	// 页内偏移
	uint32_t offset = laddr & 0xfff;

	// 一级页表访问
	uint32_t dir_base = cpu.cr3.page_directory_base << 12;
	uint32_t dir_entry = paddr_read(dir_base + dir * 4, 4);
	// 检查present位，nemu不会访问不存在的页；若缺页，代码有错
	assert(dir_entry & 0x1);

	// 二级页表访问
	uint32_t page_base = (dir_entry & 0xfffff000);
	uint32_t page_entry = paddr_read(page_base + page * 4, 4);
	// 检查present位，nemu不会访问不存在的页；若缺页，代码有错
	assert(page_entry & 0x1);

	// 物理地址
	uint32_t paddr = (page_entry & 0xfffff000) + offset;

	// printf("page mode on: laddr: 0x%x, paddr: 0x%x\n", laddr, paddr);

	return paddr;
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
