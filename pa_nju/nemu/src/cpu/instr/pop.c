#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
static void instr_execute_1op() {
    // 将栈顶的数据弹出到 opr_src.val 中
    // 手册中的ss：sp说明是sreg = ss, addr = esp
    OPERAND top;
    top.data_size = data_size;
    top.type = OPR_MEM;
    top.addr = cpu.esp;
    top.sreg = SREG_SS; 
    operand_read(&top);

    opr_src.val = top.val;
    
    // 然后将esp加上数据大小
    cpu.esp += data_size / 8;
    
    operand_write(&opr_src);
}

// 0x8f /0 pop m16/32
make_instr_impl_1op(pop, rm, v)
// 0x58-0x5f pop r
make_instr_impl_1op(pop, r, v)
// 0x1f pop ds

// 0x61 popa
make_instr_func(popa) {
    OPERAND temp;
    temp.data_size = data_size;
    temp.type = OPR_MEM;
    temp.sreg = SREG_SS;
    // pop edi
    temp.addr = cpu.esp;
    operand_read(&temp);
    cpu.edi = temp.val;
    cpu.esp += data_size / 8;
    // pop esi
    temp.addr = cpu.esp;
    operand_read(&temp);
    cpu.esi = temp.val;
    cpu.esp += data_size / 8;
    // pop ebp
    temp.addr = cpu.esp;
    operand_read(&temp);
    cpu.ebp = temp.val;
    cpu.esp += data_size / 8;
    // pop xxx (skip esp)
    cpu.esp += data_size / 8;
    // pop ebx
    temp.addr = cpu.esp;
    operand_read(&temp);
    cpu.ebx = temp.val;
    cpu.esp += data_size / 8;
    // pop edx
    temp.addr = cpu.esp;
    operand_read(&temp);
    cpu.edx = temp.val;
    cpu.esp += data_size / 8;
    // pop ecx
    temp.addr = cpu.esp;
    operand_read(&temp);
    cpu.ecx = temp.val;
    cpu.esp += data_size / 8;
    // pop eax
    temp.addr = cpu.esp;
    operand_read(&temp);
    cpu.eax = temp.val;
    cpu.esp += data_size / 8;

    return 1;
}
