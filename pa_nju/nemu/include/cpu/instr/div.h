#ifndef __INSTR_DIV_H__
#define __INSTR_DIV_H__

// 0xf6[7]
make_instr_func(idiv_rm2a_b);
// 0xf7[7]
make_instr_func(idiv_rm2a_v);
// 0xf6[6]
make_instr_func(div_rm2a_b);
// 0xf7[6]
make_instr_func(div_rm2a_v);

#endif
