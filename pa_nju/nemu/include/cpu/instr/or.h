#ifndef __INSTR_OR_H__
#define __INSTR_OR_H__
/*
Put the declarations of `or' instructions here.
*/

// 0x0c: or al, ib
make_instr_func(or_i2a_b);
// 0x0d: or eax, iv
make_instr_func(or_i2a_v);
// 0x80 1: or rm, i8
make_instr_func(or_i2rm_b);
// 0x81 1: or rm, i32
make_instr_func(or_i2rm_v);
// 0x83 1: or rm, i8, bv
make_instr_func(or_i2rm_bv);
// 0x08: or rm, r
make_instr_func(or_r2rm_b);
// 0x09: or rm, r
make_instr_func(or_r2rm_v);
// 0x0a: or r, rm
make_instr_func(or_rm2r_b);
// 0x0b: or r, rm
make_instr_func(or_rm2r_v);


#endif
