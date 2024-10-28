#ifndef __INSTR_INT_H__
#define __INSTR_INT_H__
/*
Put the declarations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/
// 0xcc是int 3，只能在调试模式下使用
// 0xcd是int n，n是立即数，触发中断n, 这里只实现n=0x80的情况，因此也只需要实现int n
// 0xcd 80
make_instr_func(int_);

#endif
