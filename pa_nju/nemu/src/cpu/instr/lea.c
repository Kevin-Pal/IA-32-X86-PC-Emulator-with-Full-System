#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/

// 0x8d: lea Gv, M
// 没有单独解码m2r的，但m的解码方式应该和rm一样
// 所以用rm2r的解码方式
static void instr_execute_2op() 
{
    // 要将src的地址长度匹配到dest的数据长度
    // 最终结果应该是把src.addr赋值给dest.val
    if(opr_dest.data_size == 16)
    {
        // Truncate_to_16bits(Addr(m))
        opr_src.addr = opr_src.addr & 0xffff;
    }
    opr_dest.val = opr_src.addr;
    operand_write(&opr_dest);
   
}

make_instr_impl_2op(lea, rm, r, v)