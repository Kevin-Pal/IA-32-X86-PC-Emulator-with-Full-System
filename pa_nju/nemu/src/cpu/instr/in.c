#include "cpu/instr.h"
#include "device/port_io.h"
/*
Put the implementations of `in' instructions here.
*/

// 0xec
// in, from port[bx] to al
// port号0-65535, 16位！一个port才是8位！
make_instr_func(in_b)
{
    OPERAND port;
    port.data_size = 16;
    port.type = OPR_REG;
    port.addr = REG_DX;
    operand_read(&port);
    // 打印port.val和cpu.edx & 0xffff
    // printf("port.val: 0x%x, cpu.edx & 0xffff: 0x%x\n", port.val, cpu.edx & 0xffff);
    uint32_t data = pio_read(port.val, 1);
    

    OPERAND al;
    al.data_size = 8;
    al.type = OPR_REG;
    al.addr = REG_AL;
    al.val = data;
    operand_write(&al);

    return 1;
}

// 0xed
// in, from port[bx] to ax
make_instr_func(in_v)
{
    OPERAND port;
    port.data_size = 16;
    port.type = OPR_REG;
    port.addr = REG_DX;
    operand_read(&port);
    uint32_t data = pio_read(port.val, data_size / 8);

    OPERAND ax;
    ax.data_size = data_size;
    ax.type = OPR_REG;
    ax.addr = REG_AX;
    ax.val = data;
    operand_write(&ax);

    return 1;
}


