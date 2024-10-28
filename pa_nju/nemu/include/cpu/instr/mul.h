#ifndef __INSTR_MUL_H__
#define __INSTR_MUL_H__

// 0xf6[4]
make_instr_func(mul_rm2a_b);
// 0xf7[4]
make_instr_func(mul_rm2a_v);

// 0x0f 0xaf /r
make_instr_func(imul_rm2r_v);
// 0xf6[5]
make_instr_func(imul_rm2a_b);
// 0xf7[5]
make_instr_func(imul_rm2a_v);
// 0x69 /r
make_instr_func(imul_irm2r_v);
// 0x6b /r
make_instr_func(imul_i8rm2r_v);

#endif
