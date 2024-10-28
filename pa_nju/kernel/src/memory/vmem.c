#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000	// 正好是一页开始的地方
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PT_SIZE - 1) / PT_SIZE) // number of page tables to cover the vmem

PDE *get_updir();

void create_video_mapping()
{

	/* TODO: create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memeory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */

	// panic("please implement me");
	// 也就是在页表中增加几项，使得虚拟地址0xa0000到0xa0000+SCR_SIZE映射到物理地址0xa0000到0xa0000+SCR_SIZE
	// 注意按页对齐，二级页表，用定义好的宏
	PDE *pdir = get_updir();	// 页目录
	uint32_t table_idx = 0;
	uint32_t frame_idx = VMEM_ADDR/PAGE_SIZE;

	memset(pdir, 0, sizeof(PDE) * NR_PT); // 先清空页目录

	// fill PDES
	pdir->val = make_pde(pdir->page_frame); // 页目录指向页表

	PTE *ptable = (PTE *)(((pdir->page_frame << 12) & 0xfffff000) + frame_idx * sizeof(PTE)); // 页表

	// fill PTES
	for (table_idx = 0; table_idx < (SCR_SIZE / PAGE_SIZE) + 1; table_idx++)
	{
		ptable->val = make_pte(frame_idx << 12); // 页表指向物理页
		frame_idx++;
		ptable++;
	}

	
}

void video_mapping_write_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		buf[i] = i;
	}
}

void video_mapping_read_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		// Log("buf[%d] = %d", i, buf[i]);
		assert(buf[i] == i);
	}
}

void video_mapping_clear()
{
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}
