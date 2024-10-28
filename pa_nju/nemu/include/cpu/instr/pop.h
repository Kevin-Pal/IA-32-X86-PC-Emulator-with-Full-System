#ifndef __INSTR_POP_H__
#define __INSTR_POP_H__
/*
Put the declarations of `pop' instructions here.
*/
// 0x8f
make_instr_func(pop_rm_v);
// 0x58-0x5f
make_instr_func(pop_r_v);

// 0x61
make_instr_func(popa);

#endif
