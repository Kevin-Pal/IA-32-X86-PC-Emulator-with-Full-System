#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/
static void instr_execute_1op() 
{
    // opr_src已经被指明了数据类型和地址，能正确读eax到edi的值
    operand_read(&opr_src);
    cpu.esp -= data_size / 8;
    // 特别注意此处是data_size / 8，而不是opr_src.data_size / 8
    // 因为opr_src有8位的情况，但push和pop指令只能操作16位和32位
    // 所以取data_size才是对8/16/32都正确的处理

    // 通过改变opr_src的地址，完成对esp的写入
    // 注意栈是倒着长在内存里的，然后esp只是一个32位寄存器，里面放的是栈顶地址
    // 所以esp的值是一个内存地址

    // 此处也特别注意要额外进行一次datasize的赋值！！！
    // 同样因为要处理8位的情况，此时栈顶下移了2或4个字节
    // 如果仍取opr_src.data_size，则operand write时只对低8位进行了赋值
    // 但高位却不一定是0！！仍可能放置的是上一次退栈的结果！
    opr_src.data_size = data_size;

    opr_src.addr = cpu.esp;
    opr_src.type = OPR_MEM;
    opr_src.sreg = SREG_SS; // 这个尚不能确定
    
    operand_write(&opr_src); 
}

// 0xff /6 push m v
make_instr_impl_1op(push, rm, v)

// 考虑把0x50-0x57的push指令合并成一个宏，指令名称自定义为push_[register name]
// 例如push_eax, push_ecx, push_edx, push_ebx, push_esp, push_ebp, push_esi, push_edi
// 操作数类型为r，suffix为v

// 50   push   %eax
make_instr_impl_1op(push_eax, r, v)
// 51   push   %ecx
make_instr_impl_1op(push_ecx, r, v)
// 52   push   %edx
make_instr_impl_1op(push_edx, r, v)
// 53   push   %ebx
make_instr_impl_1op(push_ebx, r, v)
// 54   push   %esp
make_instr_impl_1op(push_esp, r, v)
// 55   push   %ebp
make_instr_impl_1op(push_ebp, r, v)
// 56   push   %esi
make_instr_impl_1op(push_esi, r, v)
// 57   push   %edi
make_instr_impl_1op(push_edi, r, v)
// 其实统一写成一个宏也可以，但是这样更清晰

// 0x6a: push ib
make_instr_impl_1op(push, i, b)
// 0x68: push iv
make_instr_impl_1op(push, i, v)

// 0x0e, 0x16, 0x1e, 0x06, 0x0fA0, 0x0fA8 TBD

// 0x60: pusha
make_instr_func(pusha)
{
    if (data_size == 16)
    {
        uint32_t temp_esp = cpu.esp;
        cpu.esp -= 2;
        vaddr_write(cpu.esp, 2, 2, cpu.gpr[REG_AX]._16);
        cpu.esp -= 2;
        vaddr_write(cpu.esp, 2, 2, cpu.gpr[REG_CX]._16);
        cpu.esp -= 2;
        vaddr_write(cpu.esp, 2, 2, cpu.gpr[REG_DX]._16);
        cpu.esp -= 2;
        vaddr_write(cpu.esp, 2, 2, cpu.gpr[REG_BX]._16);
        cpu.esp -= 2;
        vaddr_write(cpu.esp, 2, 2, temp_esp);
        cpu.esp -= 2;
        vaddr_write(cpu.esp, 2, 2, cpu.gpr[REG_BP]._16);
        cpu.esp -= 2;
        vaddr_write(cpu.esp, 2, 2, cpu.gpr[REG_SI]._16);
        cpu.esp -= 2;
        vaddr_write(cpu.esp, 2, 2, cpu.gpr[REG_DI]._16);
    }
    else
    {
        uint32_t temp_esp = cpu.esp;
        cpu.esp -= 4;
        vaddr_write(cpu.esp, 2, 4, cpu.gpr[REG_EAX]._32);
        cpu.esp -= 4;
        vaddr_write(cpu.esp, 2, 4, cpu.gpr[REG_ECX]._32);
        cpu.esp -= 4;
        vaddr_write(cpu.esp, 2, 4, cpu.gpr[REG_EDX]._32);
        cpu.esp -= 4;
        vaddr_write(cpu.esp, 2, 4, cpu.gpr[REG_EBX]._32);
        cpu.esp -= 4;
        vaddr_write(cpu.esp, 2, 4, temp_esp);
        cpu.esp -= 4;
        vaddr_write(cpu.esp, 2, 4, cpu.gpr[REG_EBP]._32);
        cpu.esp -= 4;
        vaddr_write(cpu.esp, 2, 4, cpu.gpr[REG_ESI]._32);
        cpu.esp -= 4;
        vaddr_write(cpu.esp, 2, 4, cpu.gpr[REG_EDI]._32);
    }

    return 1;
}