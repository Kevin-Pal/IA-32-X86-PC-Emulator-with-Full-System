#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
	// printf("Please implement raise_intr()");
	// fflush(stdout);
	// assert(0);

	// protect the return address
	// push EFLAGS, CS, EIP
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eflags.val);
	cpu.esp -= 4;	// push CS, CS is 16 bits!
	vaddr_write(cpu.esp, SREG_SS, 2, cpu.cs.val);
	cpu.esp -= 4;
	vaddr_write(cpu.esp, SREG_SS, 4, cpu.eip);

	// find the IDT entry
	uint32_t idt_addr = cpu.idtr.base + intr_no * 8;
	GateDesc gate_desc;
	gate_desc.val[0] = laddr_read(idt_addr, 4);
	gate_desc.val[1] = laddr_read(idt_addr + 4, 4);

	// clear IF if it is an interrupt
	if (gate_desc.type == 0xe)
	{
		cpu.eflags.IF = 0;
	}

	// set CS:EIP, need to reload CS with load_sreg()
	cpu.cs.val = gate_desc.selector;
	load_sreg(SREG_CS);
	cpu.eip = gate_desc.offset_15_0 + (gate_desc.offset_31_16 << 16);

	return;

#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
