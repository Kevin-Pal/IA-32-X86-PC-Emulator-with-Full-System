#ifndef __INSTR_TEST_H__
#define __INSTR_TEST_H__
/*
Put the declarations of `test' instructions here.
*/
make_instr_func(test_r2rm_b);
make_instr_func(test_r2rm_v);

// 0xa8
make_instr_func(test_i2a_b);
// 0xa9
make_instr_func(test_i2a_v);
// 0xf6 0
make_instr_func(test_i2rm_b);
// 0xf7 0
make_instr_func(test_i2rm_v);

#endif
