#ifndef __INSTR_DEC_H__
#define __INSTR_DEC_H__
/*
Put the declarations of `dec' instructions here.
*/
// 0x fe /1
make_instr_func(dec_rm_b);
// 0x ff /1
make_instr_func(dec_rm_v);
// 0x 48 + rw
make_instr_func(dec_r_v);

#endif
