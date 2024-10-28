#include "cpu/instr.h"
/*
Put the implementations of `and' instructions here.
*/
static void instr_execute_2op() 
{
    operand_read(&opr_src);
    operand_read(&opr_dest);

    opr_src.val = sign_ext(opr_src.val, opr_src.data_size);

    opr_dest.val = alu_and(opr_src.val, opr_dest.val, data_size);
    operand_write(&opr_dest);
}


// 0x24: and al, ib
make_instr_impl_2op(and, i, a, b)
// 0x25: and eax, iv
make_instr_impl_2op(and, i, a, v)
// 0x80: and eb, ib
make_instr_impl_2op(and, i, rm, b)
// 0x81: and ev, iv
make_instr_impl_2op(and, i, rm, v)
// 0x83: and ev, ibv
make_instr_impl_2op(and, i, rm, bv)
// 0x20: and r, rm
make_instr_impl_2op(and, r, rm, b)
// 0x21: and r, rm
make_instr_impl_2op(and, r, rm, v)
// 0x22: and rm, r
make_instr_impl_2op(and, rm, r, b)
// 0x23: and rm, r
make_instr_impl_2op(and, rm, r, v)
