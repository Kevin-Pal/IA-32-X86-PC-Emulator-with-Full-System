#include "cpu/instr.h"
/*
Put the implementations of `sub' instructions here.
*/
// for sub, do dest = dest - src
static void instr_execute_2op() 
{
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_sub(opr_src.val, opr_dest.val, opr_dest.data_size);
    operand_write(&opr_dest);
}

// 0x2c: sub al, ib
// 必须为了al单独写一遍
make_instr_func(sub_i2al_b)
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

    print_asm_2("sub", "b", len, &imm, &a); 
    // instr_execute_2op()
    operand_read(&imm);
    operand_read(&a);
    // need al - imm
    a.val = alu_sub(imm.val, a.val, a.data_size);
    operand_write(&a);

    return len;
}

// 0x2d: sub eax, iv
// 同上，这里不能再用宏
make_instr_func(sub_i2eax_v)
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

    print_asm_2("sub", "v", len, &imm, &a); 
    // instr_execute_2op()
    operand_read(&imm);
    operand_read(&a);
    // need eax - imm
    a.val = alu_sub(imm.val, a.val, a.data_size);
    operand_write(&a);

    return len;
}

// 0x80: sub rm, ib
make_instr_impl_2op(sub, i, rm, b)
// 0x81: sub rm, iv
make_instr_impl_2op(sub, i, rm, v)

// 0x83: sub rm, ibv
// 单独写
make_instr_func(sub_i2rm_bv)
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

    print_asm_2("sub", "bv", len, &imm, &rm); 
    // instr_execute_2op()
    operand_read(&imm);
    operand_read(&rm);
    imm.val = sign_ext(imm.val, imm.data_size);
    // need rm - imm
    rm.val = alu_sub(imm.val, rm.val, rm.data_size);
    operand_write(&rm);

    return len;
}

// 0x28: sub r/m8, r8
make_instr_impl_2op(sub, r, rm, b)
// 0x29: sub r/m16/32, r16/32
make_instr_impl_2op(sub, r, rm, v)
// 0x2a: sub r8, r/m8
make_instr_impl_2op(sub, rm, r, b)
// 0x2b: sub r16/32, r/m16/32
make_instr_impl_2op(sub, rm, r, v)
