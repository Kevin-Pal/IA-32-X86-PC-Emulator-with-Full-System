#ifndef __INSTR_CALL_H__
#define __INSTR_CALL_H__
/*
Put the declarations of `call' instructions here.
*/
// 0xe8
make_instr_func(call_near);
// 0xff/2
make_instr_func(call_rm_v);
// 0xff/3
make_instr_func(call_m_v);
// 0x9a
make_instr_func(call_far);

#endif
