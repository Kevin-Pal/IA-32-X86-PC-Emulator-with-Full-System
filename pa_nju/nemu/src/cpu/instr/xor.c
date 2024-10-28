#include "cpu/instr.h"
/*
Put the implementations of `xor' instructions here.
*/
static void instr_execute_2op() 
{
    operand_read(&opr_src);
    operand_read(&opr_dest);
    opr_src.val = sign_ext(opr_src.val, opr_src.data_size);
    opr_dest.val = alu_xor(opr_src.val, opr_dest.val, data_size);
    operand_write(&opr_dest);
}

// 0x34: xor al, ib
make_instr_impl_2op(xor, i, a, b)
// 0x35: xor eax, iv
make_instr_impl_2op(xor, i, a, v)
// 0x80 /6: xor eb, ib
make_instr_impl_2op(xor, i, rm, b)
// 0x81 /6: xor ev, iv
make_instr_impl_2op(xor, i, rm, v)
// 0x83 /6: xor ev, ibv
make_instr_impl_2op(xor, i, rm, bv)
// 0x30: xor rm, r
make_instr_impl_2op(xor, r, rm, b)
// 0x31: xor rm, r
make_instr_impl_2op(xor, r, rm, v)
// 0x32: xor r, rm
make_instr_impl_2op(xor, rm, r, b)
// 0x33: xor r, rm
make_instr_impl_2op(xor, rm, r, v)