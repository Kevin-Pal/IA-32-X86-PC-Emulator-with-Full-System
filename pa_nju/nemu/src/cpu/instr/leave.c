#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
uint32_t pop()
{
    OPERAND mem;
    mem.data_size = data_size;
    mem.type = OPR_MEM;
    mem.addr = cpu.esp;
    mem.sreg = SREG_SS;
    operand_read(&mem);
    cpu.esp += data_size / 8;
    return mem.val;
}

// 0xc9: leave
make_instr_func(leave)
{
    cpu.esp = cpu.ebp;
    cpu.ebp = pop();
    return 1;
}