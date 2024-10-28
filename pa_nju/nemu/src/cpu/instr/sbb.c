#include "cpu/instr.h"
/*
Put the implementations of `sbb' instructions here.
*/
static void instr_execute_2op()
{
    operand_read(&opr_src);
    operand_read(&opr_dest);

    opr_src.val = sign_ext(opr_src.val, opr_src.data_size);

    opr_dest.val = alu_sbb(opr_src.val, opr_dest.val, data_size);
    operand_write(&opr_dest);
}

// 0x1c sbb al, ib
make_instr_impl_2op(sbb, i, a, b)
// 0x1d sbb eax, iv
make_instr_impl_2op(sbb, i, a, v)
// 0x80 /3 sbb rm, ib
make_instr_impl_2op(sbb, i, rm, b)
// 0x81 /3 sbb rm, iv
make_instr_impl_2op(sbb, i, rm, v)
// 0x83 /3 sbb rm, ibv
make_instr_impl_2op(sbb, i, rm, bv)
// 0x18 sbb r/m, r
make_instr_impl_2op(sbb, r, rm, b)
// 0x19 sbb r/m, r
make_instr_impl_2op(sbb, r, rm, v)
// 0x1a sbb r, r/m
make_instr_impl_2op(sbb, rm, r, b)
// 0x1b sbb r, r/m
make_instr_impl_2op(sbb, rm, r, v)