#include "cpu/instr.h"
/*
Put the implementations of `cmp' instructions here.
*/
// for cmp, do alu_sub, which is dest - src
static void instr_execute_2op() 
{
	operand_read(&opr_src);
	operand_read(&opr_dest);
	alu_sub(opr_src.val, opr_dest.val, opr_dest.data_size);
}


// 0x38-0x3d
// e for rm, g for r, a for a(direct address), i for i(imm)
// 0x38: cmp eb, gb
make_instr_impl_2op(cmp,r,rm,b) // should be rm - r; in this case, rm is the dest, r is the src
// 0x39: cmp ev, gv
make_instr_impl_2op(cmp,r,rm,v)
// 0x3a: cmp gb, eb
make_instr_impl_2op(cmp,rm,r,b)
// 0x3b: cmp gv, ev
make_instr_impl_2op(cmp,rm,r,v)
// 0x3c: cmp al, ib
// 0x3c & 0x07 = 0011 1100 & 0000 0111 = 0000 0100， 但0100不是eax的编号（eax编号是0），这里不能再用宏
make_instr_func(cmp_i2al_b)
{
	int len = 1;
	OPERAND imm, a;
	// decode_data_size_b
	imm.data_size = 8;
	a.data_size = 8;
	// decode_operand_i2a, a is al, address of which is 0
	imm.type = OPR_IMM;
	imm.addr = eip + 1;
	imm.sreg = SREG_CS;
	a.type = OPR_REG;
	a.addr = 0;
	a.sreg = SREG_CS;

	len += 1;

	print_asm_2("cmp", "b", len, &imm, &a); 
	// instr_execute_2op()
	operand_read(&imm);
	operand_read(&a);
	// need al - imm
	alu_sub(imm.val, a.val, a.data_size);

	return len;
}
// 0x3d: cmp eax, iv
// 同上，这里不能再用宏
make_instr_func(cmp_i2eax_v)
{
	int len = 1;
	OPERAND imm, a;
	// decode_data_size_v
	imm.data_size = data_size;
	a.data_size = data_size;
	// decode_operand_i2a, a is eax, address of which is 0
	imm.type = OPR_IMM;
	imm.addr = eip + 1;
	imm.sreg = SREG_CS;
	a.type = OPR_REG;
	a.addr = 0;
	a.sreg = SREG_CS;

	len += data_size / 8;

	print_asm_2("cmp", "v", len, &imm, &a); 
	// instr_execute_2op()
	operand_read(&imm);
	operand_read(&a);
	// need eax - imm
	alu_sub(imm.val, a.val, a.data_size);

	return len;
}

// 0x80 7 cmp eb, ib
make_instr_impl_2op(cmp, i, rm, b)

// 0x81 7 cmp ev, iv
make_instr_impl_2op(cmp, i, rm, v)

// 0x83 7
// bv only set the data size of opr_src to 8 and opr_dest to data_size
// take care of the sign extension!
// 因为在alu_sub函数中，我们并未考虑操作数长度不一样的情况（实际上sub指令本身是具有处理不等长操作数做位扩展的能力的），
// 所以此处必须手动考虑，但也没必要为这一种特殊情况在instr_execute_2op() 中添加额外的判断，单独在写一个函数吧
// make_instr_impl_2op(cmp, i, rm, bv)
make_instr_func(cmp_i2rm_bv)
{
	int len = 1;

	OPERAND rm, imm;
	// decode_data_size_bv
	imm.data_size = 8;
	rm.data_size = data_size;
	// decode_operand_i2rm
	len += modrm_rm(eip + 1, &rm);
	imm.type = OPR_IMM;
	imm.addr = eip + len;
	imm.sreg = SREG_CS;
	len += imm.data_size / 8;

	
	// instr_execute_2op()
	operand_read(&rm);
	operand_read(&imm);
	// need rm - sign_ext(imm)

	// 注意这个sign_ext(x,datasize)函数是把datasize位的x的最高位扩展到32位，而不是把x的最高位扩展到datasize位！！！！！
	// extend imm to datasize bits
	// 这里统一扩展到32位就行，alu_sub只接受32位无符号形参，当datasize为16位时alu_sub会自动把高16位数据清零
	imm.val = sign_ext(imm.val, imm.data_size);
	
	// print_asm_2会重复触发operand_read，如果此前对operand.val进行了额外的操作，此时就会失效！！！因此注意放置的位置
	
	// printf("imm_sign_ext_value: %x\n", imm.val);
	// printf("imm_data_size: %d\n", imm.data_size);
	alu_sub(imm.val, rm.val, rm.data_size);
	print_asm_2("cmp", "bv", len, &imm, &rm);

	return len;
}