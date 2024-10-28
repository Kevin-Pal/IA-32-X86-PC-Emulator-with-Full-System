#ifndef __INSTR_PUSH_H__
#define __INSTR_PUSH_H__
/*
Put the declarations of `push' instructions here.
*/
// 0x50-0x57
make_instr_func(push_eax_r_v);
make_instr_func(push_ecx_r_v);
make_instr_func(push_edx_r_v);
make_instr_func(push_ebx_r_v);
make_instr_func(push_esp_r_v);
make_instr_func(push_ebp_r_v);
make_instr_func(push_esi_r_v);
make_instr_func(push_edi_r_v);

// 0xff/6
make_instr_func(push_rm_v);
// 0x6a
make_instr_func(push_i_b);
// 0x68
make_instr_func(push_i_v);

// pusha
// 0x60
make_instr_func(pusha);
#endif
