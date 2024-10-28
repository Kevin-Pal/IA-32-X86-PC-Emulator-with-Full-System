#include "cpu/instr.h"
/*
Put the implementations of `not' instructions here.
*/

static void instr_execute_1op() 
{
    operand_read(&opr_src);
    opr_src.val = ~opr_src.val;
    operand_write(&opr_src);
}


// 0xf6 /2: not r/m8
make_instr_impl_1op(not, rm, b)
// 0xf7 /2: not r/m16/32
make_instr_impl_1op(not, rm, v)