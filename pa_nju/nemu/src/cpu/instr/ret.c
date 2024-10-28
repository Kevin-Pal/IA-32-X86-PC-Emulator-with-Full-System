#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/

extern uint32_t pop();


// 0xc3 ret_near
make_instr_func(ret_near)
{
    print_asm_0("ret_near", "", 1);
    cpu.eip = pop();
    return 0;
}

// 0xc2 ret_near_imm16
make_instr_func(ret_near_imm16)
{
    OPERAND imm;
    imm.data_size = 16;
    imm.type = OPR_IMM;
    imm.addr = eip + 1;
    imm.sreg = SREG_CS;

    print_asm_1("ret_near", "imm16", 3, &imm);

    operand_read(&imm);
    
    cpu.eip = pop();
    cpu.esp += imm.val;

    return 0;
}

// 0xcb ret_far
make_instr_func(ret_far)
{
    print_asm_0("ret_far", "", 1);
    cpu.eip = pop();
    // cpu.cs.val = pop(); // unknown
    return 0;
}

// 0xca ret_imm16_far
make_instr_func(ret_far_imm16)
{
    OPERAND imm;
    imm.data_size = 16;
    imm.type = OPR_IMM;
    imm.addr = eip + 1;
    imm.sreg = SREG_CS;

    print_asm_1("ret_far", "imm16", 3, &imm);

    operand_read(&imm);
    
    cpu.eip = pop();
    // cpu.cs.val = pop(); // unknown
    cpu.esp += imm.val;
    
    return 0;
}