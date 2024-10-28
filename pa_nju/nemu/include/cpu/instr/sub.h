#ifndef __INSTR_SUB_H__
#define __INSTR_SUB_H__
/*
Put the declarations of `sub' instructions here.
*/
// 0x2c
make_instr_func(sub_i2al_b);
// 0x2d
make_instr_func(sub_i2eax_v);
// 0x80
make_instr_func(sub_i2rm_b);
// 0x81
make_instr_func(sub_i2rm_v);
// 0x83
make_instr_func(sub_i2rm_bv);
// 0x28
make_instr_func(sub_r2rm_b);
// 0x29
make_instr_func(sub_r2rm_v);
// 0x2a
make_instr_func(sub_rm2r_b);
// 0x2b
make_instr_func(sub_rm2r_v);




#endif
