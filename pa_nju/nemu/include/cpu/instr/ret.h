#ifndef __INSTR_RET_H__
#define __INSTR_RET_H__
/*
Put the declarations of `ret' instructions here.
*/
// 0xc3 ret_near
make_instr_func(ret_near);
// 0xc2 ret_near_imm16
make_instr_func(ret_near_imm16);
// 0xcb ret_far
make_instr_func(ret_far);
// 0xca ret_far_imm16
make_instr_func(ret_far_imm16);

#endif
