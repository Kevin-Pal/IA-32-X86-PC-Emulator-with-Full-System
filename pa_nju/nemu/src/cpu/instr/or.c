#include "cpu/instr.h"
/*
Put the implementations of `or' instructions here.
*/
static void instr_execute_2op() {
    operand_read(&opr_src);
    operand_read(&opr_dest);
    opr_src.val = sign_ext(opr_src.val, opr_src.data_size);
    opr_dest.val = alu_or(opr_src.val, opr_dest.val, data_size);
    operand_write(&opr_dest);
}
// 0x0c: or al, ib
make_instr_impl_2op(or, i, a, b);
// 0x0d: or eax, iv
make_instr_impl_2op(or, i, a, v);
// 0x80 1: or rm, i8
make_instr_impl_2op(or, i, rm, b);
// 0x81 1: or rm, i32
make_instr_impl_2op(or, i, rm, v);
// 0x83 1: or rm, i8, bv
make_instr_impl_2op(or, i, rm, bv);
// 0x08: or rm, r
make_instr_impl_2op(or, r, rm, b);
// 0x09: or rm, r
make_instr_impl_2op(or, r, rm, v);
// 0x0a: or r, rm
make_instr_impl_2op(or, rm, r, b);
// 0x0b: or r, rm
make_instr_impl_2op(or, rm, r, v);