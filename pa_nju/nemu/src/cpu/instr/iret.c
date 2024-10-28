#include "cpu/instr.h"
/*
Put the implementations of `iret' instructions here.
*/
// 0xcf
make_instr_func(iret) {
    // check pe val
    // if (cpu.cr0.pe != 0) {
    //     printf("iret: pe = 0\n");
    //     assert(0);
    // }
    // 似乎不用区分实地址模式和保护模式

    // pop eip
    OPERAND m;
    m.data_size = data_size;
    m.type = OPR_MEM;
    m.addr = cpu.esp;
    m.sreg = SREG_SS;
    operand_read(&m);
    cpu.eip = m.val;
    cpu.esp += data_size / 8;

    // pop cs
    m.addr = cpu.esp;
    operand_read(&m);
    cpu.cs.val = m.val;
    cpu.esp += data_size / 8;

    // pop eflags
    m.addr = cpu.esp;
    operand_read(&m);
    cpu.eflags.val = m.val;
    cpu.esp += data_size / 8;

    print_asm_0("iret", "", 1);
    return 0;   // eip已经被修改，不需要返回值
}