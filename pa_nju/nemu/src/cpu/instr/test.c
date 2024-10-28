#include "cpu/instr.h"
/*
Put the implementations of `test' instructions here.
*/
static void instr_execute_2op() 
{
    operand_read(&opr_src);
    operand_read(&opr_dest);
    alu_and(opr_src.val, opr_dest.val, opr_dest.data_size);
    // test does not write back to opr_dest
}

// 0x84: test eb, gb
make_instr_impl_2op(test, r, rm, b)
// 0x85: test ev, gv
make_instr_impl_2op(test, r, rm, v)
// 0xa8: test al, ib
make_instr_impl_2op(test, i, a, b)
// 0xa9: test eax, iv
make_instr_impl_2op(test, i, a, v)
// 0xf6 /0: test rm, ib
make_instr_impl_2op(test, i, rm, b)
// 0xf7 /0: test rm, iv
make_instr_impl_2op(test, i, rm, v)