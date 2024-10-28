#include "cpu/instr.h"
/*
Put the implementations of `add' instructions here.
*/
// for add, do dest = dest + src
static void instr_execute_2op() 
{
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_add(opr_src.val, opr_dest.val, opr_dest.data_size);
    operand_write(&opr_dest);
}

// 0x04: add al, ib
// 必须为了al单独写一遍
make_instr_func(add_i2al_b)
{
    int len = 1;
    OPERAND imm, a;
    // decode_data_size_b
    imm.data_size = 8;
    a.data_size = 8;
    // decode_operand_i2a, a is al, address of which is 0
    imm.type = OPR_IMM;
    imm.addr = eip + 1;
    imm.sreg = SREG_CS;
    a.type = OPR_REG;
    a.addr = 0;
    a.sreg = SREG_CS;

    len += 1;

    print_asm_2("add", "b", len, &imm, &a); 
    // instr_execute_2op()
    operand_read(&imm);
    operand_read(&a);
    // need al + imm
    a.val = alu_add(imm.val, a.val, a.data_size);
    operand_write(&a);

    return len;
}

// 0x05: add eax, iv
// 同上，这里不能再用宏
make_instr_func(add_i2eax_v)
{
    int len = 1;
    OPERAND imm, a;
    // decode_data_size_v
    imm.data_size = data_size;
    a.data_size = data_size;
    // decode_operand_i2a, a is eax, address of which is 0
    imm.type = OPR_IMM;
    imm.addr = eip + 1;
    imm.sreg = SREG_CS;
    a.type = OPR_REG;
    a.addr = 0;
    a.sreg = SREG_CS;

    len += data_size / 8;

    print_asm_2("add", "v", len, &imm, &a); 
    // instr_execute_2op()
    operand_read(&imm);
    operand_read(&a);
    // need eax + imm
    a.val = alu_add(imm.val, a.val, a.data_size);
    operand_write(&a);

    return len;
}

// 0x80: add rm8, ib
make_instr_impl_2op(add, i, rm, b)
// 0x81: add rm16/32, iv
make_instr_impl_2op(add, i, rm, v)

// 0x83: add rm16/32, ib
// 需要单独编写，因为alu_add()函数中，对于不同长度的操作数，不会自动做位扩展
make_instr_func(add_i2rm_bv)
{
    int len = 1;
    OPERAND imm, rm;
    // decode_data_size_v
    imm.data_size = 8;
    rm.data_size = data_size;
    // decode_operand_i2rm
    len += modrm_rm(eip + 1, &rm);
	imm.type = OPR_IMM;
	imm.addr = eip + len;
	imm.sreg = SREG_CS;
	len += imm.data_size / 8;


    print_asm_2("add", "bv", len, &imm, &rm); 
    // instr_execute_2op()
    operand_read(&imm);
    operand_read(&rm);
    imm.val = sign_ext(imm.val, imm.data_size);
    // need rm + imm
    rm.val = alu_add(imm.val, rm.val, rm.data_size);
    operand_write(&rm);

    return len;
}

// 0x00: add rm8, r8
make_instr_impl_2op(add, r, rm, b)
// 0x01: add rm16/32, r16/32
make_instr_impl_2op(add, r, rm, v)
// 0x02: add r8, rm8
make_instr_impl_2op(add, rm, r, b)
// 0x03: add r16/32, rm16/32
make_instr_impl_2op(add, rm, r, v)