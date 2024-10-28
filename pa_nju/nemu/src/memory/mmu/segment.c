#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */

	// get base and limit from the invisible part of the segment register
	uint32_t base = cpu.segReg[sreg].base;
	uint32_t limit = cpu.segReg[sreg].limit;

	// 颗粒度是4KB，所以limit是以4KB为单位的，但是在segment register中都应该是以字节为单位的（为32位int），故此处不必换算
	// limit = (limit << 12) + 0xfff;

	// 打印offset, base, limit
	if (offset > limit) {
		printf("offset: %x, base: %x, limit: %x\n", offset, base, limit);
	}
	// check the limit
	assert(offset <= limit);
	// check the base
	assert(base == 0);	// 扁平模式下，base应该是0

	// 对优先级的检查留在load_sreg中进行

	// return the linear address
	return base + offset;

}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */

	// sreg是段寄存器的索引，从0到5对应es, cs, ss, ds, fs, gs

	// get index and RPL from the visible part of the segment register
	uint32_t index = cpu.segReg[sreg].index;
	uint32_t RPL = cpu.segReg[sreg].rpl;

	// get the Segment Descriptor from the GDT
	SegDesc segdesc;
	// GDTR的base是GDT的基地址, 是一个线性地址
	uint32_t base = cpu.gdtr.base;
	// 从GDT中读取Segment Descriptor，注意GDT中的每个表项是8字节，又是两个4字节的val组成的union
	// 判断溢出
	assert(base + index * 8 + 7 <= base + cpu.gdtr.limit);

	segdesc.val[0] = laddr_read(base + index * 8, 4);
	segdesc.val[1] = laddr_read(base + index * 8 + 4, 4);

	// check the privilege level
	assert(RPL <= segdesc.privilege_level);

	// check the present bit
	assert(segdesc.present == 1);

	// check the granularity
	assert(segdesc.granularity == 1);	// 扁平模式下，granularity应该是1，否则不够4GB

	// load the invisible part of the segment register
	cpu.segReg[sreg].base = (segdesc.base_31_24 << 24) + (segdesc.base_23_16 << 16) + segdesc.base_15_0;
	
	cpu.segReg[sreg].limit = (segdesc.limit_19_16 << 16) + segdesc.limit_15_0;
	assert(cpu.segReg[sreg].limit == 0xfffff); // 扁平模式下，limit应该是0xfffff，而且granularity是1，这样才能保证limit是4GB
	cpu.segReg[sreg].limit = (cpu.segReg[sreg].limit << 12) + 0xfff;

	cpu.segReg[sreg].type = segdesc.type;
	cpu.segReg[sreg].privilege_level = segdesc.privilege_level;
	cpu.segReg[sreg].soft_use = segdesc.soft_use;

	return;
}
