#ifndef __INSTR_INC_H__
#define __INSTR_INC_H__
/*
Put the declarations of `inc' instructions here.
*/
// 0x40-0x47
make_instr_func(inc_r_v);
// 0xfe 0
make_instr_func(inc_rm_b);
// 0xff 0
make_instr_func(inc_rm_v);
#endif
