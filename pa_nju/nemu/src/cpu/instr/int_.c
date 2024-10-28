#include "cpu/intr.h"
#include "cpu/instr.h"

/*
Put the implementations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/
// 0xcc是int 3，只能在调试模式下使用
// 0xcd是int n，n是立即数，触发中断n, 这里只实现n=0x80的情况，因此也只需要实现int n
// 0xcd 80
make_instr_func(int_) {
    int len = 1, intr_no = 0;
    OPERAND imm;
    imm.type = OPR_IMM;
    imm.sreg = SREG_CS;
    imm.data_size = 8;
    imm.addr = eip + len;
    len += 1;
    operand_read(&imm);
    print_asm_1("int", "", len, &imm);
    intr_no = imm.val;
    assert(intr_no == 0x80);    // only implement int 0x80

    cpu.eip += len; 
    // 此处eip就要更新为下一条指令的地址，因为后续要压栈eip并跳转新智

    // push eflags, cs, eip
    // push eflags
    cpu.esp -= 4;
    vaddr_write(cpu.esp, 2, 4, cpu.eflags.val);
    // set IF to 0
    cpu.eflags.IF = 0;
    // set TF to 0
    cpu.eflags.TF = 0;
    // push cs, 16 bits!
    cpu.esp -= 4;
    vaddr_write(cpu.esp, 2, 2, cpu.cs.val);
    // push eip
    cpu.esp -= 4;
    vaddr_write(cpu.esp, 2, 4, cpu.eip);
    // set cs:ip
    // 按照手册，似乎intr_no还要乘4才是IDT对应的表项？？？No
    uint32_t idt_addr = cpu.idtr.base + intr_no * 8;    // 一条IDT表项占8字节
    GateDesc gate_desc;
    gate_desc.val[0] = laddr_read(idt_addr, 4);
    gate_desc.val[1] = laddr_read(idt_addr + 4, 4);
    cpu.cs.val = gate_desc.selector;
    load_sreg(SREG_CS);
    cpu.eip = gate_desc.offset_15_0 + (gate_desc.offset_31_16 << 16);
    
    // eip已经被修改，不需要返回值
    return 0;
}