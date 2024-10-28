#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/
static void instr_execute_1op() 
{
    operand_read(&opr_src);
    opr_src.val = alu_add(1, opr_src.val, opr_src.data_size);
    operand_write(&opr_src);
}

// 0xfe [0]: inc eb
make_instr_impl_1op(inc, rm, b)
// 0xff [0]: inc ev
make_instr_impl_1op(inc, rm, v)
// no 0xff [6]: print error!
// 0x40-0x47: inc rv
// 可以用宏合并，正好排列
make_instr_impl_1op(inc, r, v)