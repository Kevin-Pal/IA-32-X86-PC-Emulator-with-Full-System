#include "cpu/instr.h"
/*
Put the implementations of `adc' instructions here.
*/

static void instr_execute_2op()
{
    operand_read(&opr_src);
    operand_read(&opr_dest);

    opr_src.val = sign_ext(opr_src.val, opr_src.data_size);
   
    opr_dest.val = alu_adc(opr_src.val, opr_dest.val, opr_dest.data_size);

    operand_write(&opr_dest);
}

// 0x14: adc al, ib
make_instr_impl_2op(adc, i, a, b)
// 0x15: adc eax, iv
make_instr_impl_2op(adc, i, a, v)
// 0x80: adc rm, ib
make_instr_impl_2op(adc, i, rm, b)
// 0x81: adc rm, iv
make_instr_impl_2op(adc, i, rm, v)
// 0x83: adc rm, ibv
make_instr_impl_2op(adc, i, rm, bv)
// 0x10: adc r, rm
make_instr_impl_2op(adc, r, rm, b)
// 0x11: adc r, rm
make_instr_impl_2op(adc, r, rm, v)
// 0x12: adc rm, r
make_instr_impl_2op(adc, rm, r, b)
// 0x13: adc rm, r
make_instr_impl_2op(adc, rm, r, v)