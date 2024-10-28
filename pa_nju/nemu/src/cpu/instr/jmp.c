#include "cpu/instr.h"

// 0xe9 rel16/32 jump near
// this is a relative jump
make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        // 16 or 32 bits also depends on the data_size
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

// 0xeb rel8 jump short
// this is a relative jump, and is a 8-bit offset
make_instr_func(jmp_short_)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = 8;
        // 8 bits
        rel.addr = eip + 1;

        operand_read(&rel);

        // 可往前跳，往后跳；所以是个负数！
        int offset = sign_ext(rel.val, 8);
        print_asm_1("jmp", "", 2, &rel);

        cpu.eip += offset;

        return 2;
}

// 0xea ptr16:16/32 jump far
// 4 or 6 immediate bytes
// jump intersegment
// 远指针，左侧值为16位段选择子，右侧值为32位偏移量
// IF operand type = ptr16:16 or ptr16:32
// THEN
// IF OperandSize = 16
// THEN
// CS:IP ← ptr16:16;
// EIP ← EIP AND 0000FFFFH; (* clear upper 16 bits *)
// ELSE (* OperandSize = 32 *)
// CS:EIP ← ptr16:32;
// FI;
// FI;
make_instr_func(jmp_far)
{
        // 注意：按照小端序，ptr16是高位，ptr32是低位
        // 则ptr16在高地址，ptr32在低地址
        // 所以先读取的一定是ptr32也就是offset=eip，然后才是段选择子seg！！！

        // 指令中的立即数在CS段中
        OPERAND seg, offset;

        offset.type = OPR_IMM;
        offset.sreg = SREG_CS;
        offset.data_size = data_size;
        offset.addr = eip + 1;

        seg.type = OPR_IMM;
        seg.sreg = SREG_CS;
        seg.data_size = 16;
        seg.addr = eip + 1 + data_size / 8;

        operand_read(&seg);
        operand_read(&offset);

        // cpu.cs.val = seg.val; // unknown
        print_asm_1("jmp", "ptr16:16/32", 3 + data_size / 8, &offset);

        // set the cs
        cpu.cs.val = seg.val;
        // update invisible part of the sreg CS
        load_sreg(1);   // 1 is the index of CS in the sreg array

        // set the eip
        cpu.eip = offset.val; // 在16位时也会自动清除高16位

        return 0;
        // 跳转绝对地址时，要规避掉eip自增的问题！！！
}

// 0xff /4 jmp r/m16/32
// jmp_rm_v
make_instr_func(jmp_rm_v)
{
        int len = 1;
        OPERAND rm;
        rm.data_size = data_size;
        // decode_operand_rm
        len += modrm_rm(eip + 1, &rm);
        print_asm_1("jmp", "rm", len, &rm);
        operand_read(&rm);
        
        // jmp_v
        cpu.eip = rm.val;

        return 0;
}

// 0xff /5 jmp m16:16/32
// jmp_m_v
make_instr_func(jmp_m_v)
{
        int len = 1;

        OPERAND m,offset;
        m.data_size = 16;
        len += modrm_rm(eip + 1, &m);

        offset.data_size = data_size;
        len += modrm_rm(eip + len, &offset);
        // m16:16 should be a memory address, including both
        

        operand_read(&m);
        operand_read(&offset);

        // cpu.cs.val = m.val; // unknown
        print_asm_1("jmp", "m16:16/32", len, &offset);

        cpu.eip = offset.val;

        return 0;
}
