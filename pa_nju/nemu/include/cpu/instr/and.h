#ifndef __INSTR_AND_H__
#define __INSTR_AND_H__
/*
Put the declarations of `and' instructions here.
*/
// 0x24
make_instr_func(and_i2a_b);
// 0x25
make_instr_func(and_i2a_v);
// 0x80 4
make_instr_func(and_i2rm_b);
// 0x81 4
make_instr_func(and_i2rm_v);
// 0x83 4
make_instr_func(and_i2rm_bv);
// 0x20
make_instr_func(and_r2rm_b);
// 0x21
make_instr_func(and_r2rm_v);
// 0x22
make_instr_func(and_rm2r_b);
// 0x23
make_instr_func(and_rm2r_v);

#endif
