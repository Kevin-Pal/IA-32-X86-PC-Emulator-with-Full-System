#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
static void push_eip()
{
    OPERAND mem;
    cpu.esp -= data_size / 8;

    mem.data_size = data_size;
    mem.type = OPR_MEM;
    mem.addr = cpu.esp;
    mem.sreg = SREG_SS;
    mem.val = cpu.eip;
    
    operand_write(&mem);
}

// 0xe8: call rel16/32
make_instr_func(call_near)
{
    int len = 1;
    OPERAND rel;
    rel.data_size = data_size;
    rel.type = OPR_IMM;
    rel.addr = eip + 1;
    rel.sreg = SREG_CS;

    operand_read(&rel);
    print_asm_1("call", "rel", len + data_size / 8, &rel);

    cpu.eip += len + data_size / 8;
    // 注意在call指令中，压入栈的一定是下一条指令的地址，而不是当前指令的地址！
    // 否则将引起死循环！！！
    // 所以push前就必须完成eip自增！！！同时返回值千万要为0，因为已经自增过了
    push_eip();
    cpu.eip += rel.val;


    return 0;
}

// 0xff /2 call r/m16/32
make_instr_func(call_rm_v)
{
    int len = 1;
    OPERAND rm;
    rm.data_size = data_size;
    len += modrm_rm(eip + 1, &rm);
    operand_read(&rm);
    print_asm_1("call", "rm", len, &rm);

    cpu.eip += len;
    push_eip();
    cpu.eip = rm.val;

    return 0;
}

// 0xff /3 call m16:16/32
make_instr_func(call_m_v)
{
    int len = 1;

    
    OPERAND m,dest;
    m.data_size = 16;
    len += modrm_rm(eip + 1, &m);

    dest.data_size = data_size;
    len += modrm_rm(eip + len, &dest);

    // push_cs(); // unknown
    eip += len;
    push_eip();


    operand_read(&m);
    operand_read(&dest);

    print_asm_1("call", "m", len, &m);

    // cpu.cs.val = m.val; // unknown
    cpu.eip = dest.val;

    return 0;
}

// 0x9a call ptr16:16/32
make_instr_func(call_far)
{
    OPERAND seg, offset;
    seg.type = OPR_IMM;
    seg.sreg = SREG_CS;
    seg.data_size = 16;
    seg.addr = eip + 1;

    offset.type = OPR_IMM;
    offset.sreg = SREG_CS;
    offset.data_size = data_size;
    offset.addr = eip + 3;

    operand_read(&seg);
    operand_read(&offset);

    cpu.eip += 3 + data_size / 8;
    // push_cs(); // unknown
    push_eip();

    print_asm_1("call", "ptr16:16/32", 3 + data_size / 8, &offset);

    // cpu.cs.val = seg.val; // unknown
    cpu.eip = offset.val; // 在16位时也会自动清除高16位

    return 0;
    // 跳转绝对地址时，要规避掉eip自增的问题！！！
}