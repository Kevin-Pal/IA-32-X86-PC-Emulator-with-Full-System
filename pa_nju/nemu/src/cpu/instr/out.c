#include "cpu/instr.h"
#include "device/port_io.h"
/*
Put the implementations of `out' instructions here.
*/

// 0xee
// out, from al to port[dx]
make_instr_func(out_b)
{
    OPERAND port;
    port.data_size = 16;
    port.type = OPR_REG;
    port.addr = REG_DX;
    operand_read(&port);
    OPERAND al;
    al.data_size = 8;
    al.type = OPR_REG;
    al.addr = REG_AL;
    operand_read(&al);
    pio_write(port.val, 1, al.val);
    return 1;
}

// 0xef
// out, from ax to port[dx]
make_instr_func(out_v)
{
    OPERAND port;
    port.data_size = 16;
    port.type = OPR_REG;
    port.addr = REG_DX;
    operand_read(&port);
    OPERAND ax;
    ax.data_size = data_size;
    ax.type = OPR_REG;
    ax.addr = REG_AX;
    operand_read(&ax);
    pio_write(port.val, data_size / 8, ax.val);
    return 1;
}