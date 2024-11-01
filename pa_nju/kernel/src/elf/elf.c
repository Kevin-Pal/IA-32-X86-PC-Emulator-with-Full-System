#include "common.h"
#include "memory.h"
#include "string.h"

#include <elf.h>	// windows操作系统下估计没有这个头文件

#ifdef HAS_DEVICE_IDE
#define ELF_OFFSET_IN_DISK 0
#endif

#define STACK_SIZE (1 << 20)

void ide_read(uint8_t *, uint32_t, uint32_t);
void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph, *eph;

#ifdef HAS_DEVICE_IDE
	uint8_t buf[4096];
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
	elf = (void *)buf;	// elf指向elf头在内存中的位置
	Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	Log("ELF loading from ram disk.");
#endif

	/* Load each program segment */
	ph = (void *)elf + elf->e_phoff;
	eph = ph + elf->e_phnum;
	for (; ph < eph; ph++)
	{
		if (ph->p_type == PT_LOAD)
		{

			// remove this panic!!!
			// panic("Please implement the loader");
#ifdef HAS_DEVICE_IDE
#ifdef IA32_PAGE
			uint32_t vaddr = mm_malloc(ph->p_vaddr, ph->p_memsz);
			ide_read((void *)vaddr, ph->p_offset, ph->p_filesz);
			// copy from hard disk to memory
			// 因为不知道模拟ide的内存地址，所以一定要用ide_read的接口，而不是memcpy
			memset((void *)vaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
			// 清空剩余的内存
#else
			ide_read((void *)ph->p_vaddr, ph->p_offset, ph->p_filesz);
			memset((void *)ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
#endif


#else
#ifdef IA32_PAGE
			uint32_t vaddr = mm_malloc(ph->p_vaddr, ph->p_memsz);
			memcpy((void *)vaddr, (void *)elf + ph->p_offset, ph->p_filesz);
			// copy from ram disk to memory
			memset((void *)vaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
#else
/* TODO: copy the segment from the ELF file to its proper memory area */
			memcpy((void *)ph->p_vaddr, (void *)elf + ph->p_offset, ph->p_filesz);

/* TODO: zeror the memory area [vaddr + file_sz, vaddr + mem_sz) */
			memset((void *)ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
#endif

#endif
		

#ifdef IA32_PAGE
			/* Record the program break for future use */
			extern uint32_t brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if (brk < new_brk)
			{
				brk = new_brk;
			}
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif
	write_cr3(get_ucr3());
#endif
	return entry;
}
