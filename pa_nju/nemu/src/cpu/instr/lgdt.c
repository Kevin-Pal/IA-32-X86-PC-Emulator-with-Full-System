#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/

// 装载GDT首地址和界限到GDTR寄存器
// 操作数是一个内存地址（单操作数），这个地址指向一个6字节的数据，前两字节是界限，后四字节是基址
// 地址一定是一个线性地址，所以不能用封装好的operand_read
// 0f 01 /2
make_instr_func(lgdt) {

    int len = 1;
    OPERAND rm;
    rm.data_size = data_size;
    len += modrm_rm(eip + 1, &rm);

    operand_read(&rm);
    cpu.gdtr.limit = laddr_read(rm.addr, 2);

    if (data_size == 16) {
        cpu.gdtr.base = laddr_read(rm.addr + 2, 3) & 0x00ffffff; // 24位地址
    }
    else {
        cpu.gdtr.base = laddr_read(rm.addr + 2, 4); // 32位地址
    }
    print_asm_1("lgdt", "", len, &rm);
    return len;
}