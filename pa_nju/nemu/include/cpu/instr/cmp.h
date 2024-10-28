#ifndef __INSTR_CMP_H__
#define __INSTR_CMP_H__
/*
Put the declarations of `cmp' instructions here.
*/
// 0x80 7
make_instr_func(cmp_i2rm_b);

// 0x81 7
make_instr_func(cmp_i2rm_v);

// 0x83 7
make_instr_func(cmp_i2rm_bv);

// 0x38-0x3d
make_instr_func(cmp_r2rm_b);
make_instr_func(cmp_r2rm_v);
make_instr_func(cmp_rm2r_b);
make_instr_func(cmp_rm2r_v);
make_instr_func(cmp_i2al_b);
make_instr_func(cmp_i2eax_v);


#endif
