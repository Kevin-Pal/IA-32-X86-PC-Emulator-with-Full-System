#ifndef __INSTR_XOR_H__
#define __INSTR_XOR_H__
/*
Put the declarations of `xor' instructions here.
*/
// 0x34
make_instr_func(xor_i2a_b);
// 0x35
make_instr_func(xor_i2a_v);
// 0x80/6
make_instr_func(xor_i2rm_b);
// 0x81/6
make_instr_func(xor_i2rm_v);
// 0x83/6
make_instr_func(xor_i2rm_bv);
// 0x30
make_instr_func(xor_r2rm_b);
// 0x31
make_instr_func(xor_r2rm_v);
// 0x32
make_instr_func(xor_rm2r_b);
// 0x33
make_instr_func(xor_rm2r_v);

#endif
