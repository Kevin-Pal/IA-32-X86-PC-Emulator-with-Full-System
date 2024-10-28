#ifndef __INSTR_JMP_H__
#define __INSTR_JMP_H__

// 0xe9
make_instr_func(jmp_near);

// 0xeb
make_instr_func(jmp_short_);

// 0xea
make_instr_func(jmp_far);

// 0xff[4]
make_instr_func(jmp_rm_v);

// 0xff[5]
make_instr_func(jmp_m_v);

#endif
