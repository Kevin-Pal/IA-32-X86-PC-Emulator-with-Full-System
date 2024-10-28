#include "cpu/cpu.h"

// define opration for eflags
void set_CF_add(uint32_t result, uint32_t src, size_t data_size)
{
	// clear data in high part first and then compare the result with src
	result &= (0xFFFFFFFF >> (32 - data_size));
	src &= (0xFFFFFFFF >> (32 - data_size));
	// 是否一定需要做位扩展后再进行比较？？？
	cpu.eflags.CF = result < src;
	// this won't work for adc, need further modification
	return;
}

void set_CF_adc(uint32_t result, uint32_t src, size_t data_size)
{
	if (cpu.eflags.CF == 0)
	{
		set_CF_add(result, src, data_size);
		return;
	}
	else
	{
		// clear data in high part first and then compare the result with src
		result &= (0xFFFFFFFF >> (32 - data_size));
		src &= (0xFFFFFFFF >> (32 - data_size));
		// result couldn't equal to src if carry does't happen, as there is an extra 1
		cpu.eflags.CF = result <= src;
		return;
	}
}

void set_CF_sub(uint32_t src, uint32_t dest, size_t data_size)
{
	// clear data in high part first and then compare the dest with src (dest - src)
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	// Set if src > dest; cleared otherwise.
	cpu.eflags.CF = src > dest;
	return;
}

void set_CF_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
	if (cpu.eflags.CF == 0)
	{
		set_CF_sub(src, dest, data_size);
		return;
	}
	else
	{
		// clear data in high part first and then compare the dest with src (dest - src)
		src &= (0xFFFFFFFF >> (32 - data_size));
		dest &= (0xFFFFFFFF >> (32 - data_size));
		// Set if src >= dest; cleared otherwise.
		cpu.eflags.CF = src >= dest;
		return;
		
	}
	
	// clear data in high part first and then compare the dest with src (dest - src)
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	// Set if src > dest; cleared otherwise.
	cpu.eflags.CF = src >= dest;
	return;
}

void set_PF(uint32_t result)
{
	// parity of low-order eight bits
	int cnt = 0;
	for (int i = 0; i < 8; i++)
	{
		if ((result >> i) & 0x00000001)
			cnt++;
	}
	// even number of 1 bits; cleared otherwise
	cpu.eflags.PF = (cnt % 2 == 0);
	return;
}

void set_ZF(uint32_t result, size_t data_size)
{
	// clear data in high part first and then compare the result with 0
	result &= (0xFFFFFFFF >> (32 - data_size));
	// Set if result is zero; cleared otherwise.
	cpu.eflags.ZF = (result == 0);
	return;
}

void set_SF(uint32_t result, size_t data_size)
{
	// clear data in high part first
	result &= (0xFFFFFFFF >> (32 - data_size));
	// Set equal to the high-order bit of the result
	cpu.eflags.SF = (result >> (data_size - 1)) & 0x00000001;
	return;
}

void set_OF_adx(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{
	// this should also work for adc
	// 多了一个1，只在result为最大正数才会溢出，则src和dest必为正号
	// 此时仍然在判定范围内(注意到0也还是正数)
	// clear data in high part first
	result &= (0xFFFFFFFF >> (32 - data_size));
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	// get the sign of src, dest and result
	int sign_src = (src >> (data_size - 1)) & 0x00000001;
	int sign_dest = (dest >> (data_size - 1)) & 0x00000001;
	int sign_result = (result >> (data_size - 1)) & 0x00000001;

	if (sign_src == sign_dest)
	{
		if (sign_result != sign_src)
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	}
	else
	{
		cpu.eflags.OF = 0;
	}
	return;
}

void set_OF_sux(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{
	// this should also work for sbb
	// 多减一个1，只在result为最小负数才会溢出，则src和dest必为一正一负
	// 此时仍然在判定范围内(注意到0也还是正数)
	// clear data in high part first
	result &= (0xFFFFFFFF >> (32 - data_size));
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	// get the sign of src, dest and result
	int sign_src = (src >> (data_size - 1)) & 0x00000001;
	int sign_dest = (dest >> (data_size - 1)) & 0x00000001;
	int sign_result = (result >> (data_size - 1)) & 0x00000001;

	if (sign_src != sign_dest) // different sign
	{
		if (sign_result != sign_dest)
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	}
	else
	{
		cpu.eflags.OF = 0;
	}
	
	return;
}	

//////////////////////////////////////////////////////////////////////////
// 我们只需要实现无符号整数加减法即可。同时，考虑是否带进位和借位的加减法，我们一共需要实现四个无符号加减法函数
// 不对AF标志位进行处理！！！

// DEST ← DEST + SRC
// OF/SF/ZF/PF/CF Modified (ignore AF)
uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// result
	// When the real size of the date does not correspond to the data_size, the high part should be ignored (Don't leave things undefined!!!)-> this has influnce to the eflags
	uint32_t result = (dest & (0xFFFFFFFF >> (32 - data_size))) + (src & (0xFFFFFFFF >> (32 - data_size)));
	// change eflags
	set_CF_add(result, src, data_size);
	set_PF(result);
	set_ZF(result, data_size);
	set_SF(result, data_size);
	set_OF_adx(result, src, dest, data_size);
	// to emulate alu, must cut the result to the right size; high part should only be used for eflags
	return result & (0xFFFFFFFF >> (32 - data_size));

#endif
}

// DEST ← DEST + SRC + CF
// OF/SF/ZF/PF/CF Modified (ignore AF)
uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// result
	uint32_t result = (dest & (0xFFFFFFFF >> (32 - data_size))) + (src & (0xFFFFFFFF >> (32 - data_size))) + cpu.eflags.CF;
	// change eflags
	set_CF_adc(result, src, data_size);
	set_PF(result);
	set_ZF(result, data_size);
	set_SF(result, data_size);
	set_OF_adx(result, src, dest, data_size);
	return result & (0xFFFFFFFF >> (32 - data_size));
#endif
}

// IF SRC is a byte and DEST is a word or dword
// THEN DEST ← DEST - Sign-extend(SRC)
// ELSE DEST ← DEST - SRC
// OF/SF/ZF/PF/CF Modified (ignore AF)
uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// lack of necessary parameters, ignore the sign extend
	// if (src_date_size < dest_data_size)
	// {
	// 	src = sign_ext(src, src_date_size);
	// }

	// result
	uint32_t result = (dest & (0xFFFFFFFF >> (32 - data_size))) - (src & (0xFFFFFFFF >> (32 - data_size)));
	// change eflags
	set_CF_sub(src, dest, data_size);
	set_PF(result);
	set_ZF(result, data_size);
	set_SF(result, data_size);
	set_OF_sux(result, src, dest, data_size);

	return result & (0xFFFFFFFF >> (32 - data_size));

#endif
}

// IF SRC is a byte and DEST is a word or dword
// THEN DEST = DEST - (SignExtend(SRC) + CF)
// ELSE DEST ← DEST - (SRC + CF);
// OF/SF/ZF/PF/CF Modified (ignore AF)
uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// result
	uint32_t result = (dest & (0xFFFFFFFF >> (32 - data_size))) - ((src & (0xFFFFFFFF >> (32 - data_size))) + cpu.eflags.CF);
	// change eflags
	set_CF_sbb(src, dest, data_size);
	set_PF(result);
	set_ZF(result, data_size);
	set_SF(result, data_size);
	set_OF_sux(result, src, dest, data_size);

	return result & (0xFFFFFFFF >> (32 - data_size));

#endif
}

//////////////////////////////////////////////////////////////////////////
// IF byte-size operation
// THEN AX ← AL * r/m8 (AX contains both AL and AH)
// ELSE (* word or doubleword operation *)
// IF OperandSize = 16
// THEN DX:AX ← AX * r/m16
// ELSE (* OperandSize = 32 *)
// EDX:EAX ← EAX * r/m32
// FI;
// 简单总结就是8位乘法要16位存，16位乘法要32位存（DX高16，AX低16），32位乘法要64位存（EDX高32，EAX低32）
// OF/CF Modified ; SF/ZF/PF/AF/CF Undefined
// Undefined means that the value of the flag is not predictable after the operation
uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	uint64_t result = 0;

	// optimization for 0 and 1
	// operand has 0
	if (src == 0 || dest == 0)
	{
		cpu.eflags.CF = 0;
		cpu.eflags.OF = 0;
		return result;
	}
	// operand has 1
	if (src == 1)
	{
		cpu.eflags.CF = 0;
		cpu.eflags.OF = 0;
		return dest;
	}
	if (dest == 1)
	{
		cpu.eflags.CF = 0;
		cpu.eflags.OF = 0;
		return src;
	}
	// operand has 2 -> CF & OF are complex -> ignore

	switch (data_size)
	{
		case 8:{
			// result
			result = ((src & 0x000000FF) * (dest & 0x000000FF)) & 0x000000000000FFFF;
			// eflags (OF CF are set 1 when high part is not 0)
			if (result >>8) // AH is not 0
				{cpu.eflags.CF = 1;
				cpu.eflags.OF = 1;}
			else
				{cpu.eflags.CF = 0;
				cpu.eflags.OF = 0;}
			return result;
		}
		case 16:{
			// result
			result = ((src & 0x0000FFFF) * (dest & 0x0000FFFF)) & 0x00000000FFFFFFFF;
			// eflags (OF CF are set 1 when high part is not 0)
			if (result >>16) // DX is not 0
				{cpu.eflags.CF = 1;
				cpu.eflags.OF = 1;}
			else
				{cpu.eflags.CF = 0;
				cpu.eflags.OF = 0;}
			return result;
		}
		case 32:{
			// result
			result = ((uint64_t)src * (uint64_t)dest);
			// eflags (OF CF are set 1 when high part is not 0)
			if (result >>32) // EDX is not 0
				{cpu.eflags.CF = 1;
				cpu.eflags.OF = 1;}
			else
				{cpu.eflags.CF = 0;
				cpu.eflags.OF = 0;}
			return result;
		}
		default:
			return -1;

	}

#endif
}

// result ← multiplicand * multiplier;
// IMUL clears the overflow and carry flags under the following conditions:
// Instruction 			Form Condition for Clearing CF and OF
// r/m8 				AL = sign-extend of AL to 16 bits
// r/m16 				AX = sign-extend of AX to 32 bits
// r/m32 				EDX:EAX = sign-extend of EAX to 32 bits
// r16,r/m16 			Result exactly fits within r16
// r/32,r/m32 			Result exactly fits within r32
// r16,r/m16,imm16 		Result exactly fits within r16
// r32,r/m32,imm32 		Result exactly fits within r32
// 计算结果同MUL一样，x位乘，2x位输出结果
// 不知道指令具体形式，简单按照几位乘，结果不超过几位就clear来设计
// 如果没有发生以上情形，是否也不用手动给OF和CF赋值为1？？？
int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	src = sign_ext(src, data_size);
	dest = sign_ext(dest, data_size);

	int64_t result = 0;
	// optimization for 0 and 1
	// operand has 0
	if (src == 0 || dest == 0)
	{
		cpu.eflags.CF = 0;
		cpu.eflags.OF = 0;
		return result;
	}
	// operand has 1
	if (src == 1)
	{
		cpu.eflags.CF = 0;
		cpu.eflags.OF = 0;
		return dest;
	}
	if (dest == 1)
	{
		cpu.eflags.CF = 0;
		cpu.eflags.OF = 0;
		return src;
	}

	// src and dest are passed in the form according to the data_size, but all treated as 32 bits
	// so must be sign-extended to 32 bits first!!! src & dest could only to 32 bits multiplication
	
	
	result = (int64_t)src * (int64_t)dest;
	// 8位乘法用16位存，其余高位是置零还是不管？？？
	// 这里多的高位直接置零
	// result = result & (0xFFFFFFFFFFFFFFFF >> (64 - 2 * data_size));
	// 这里多的高位直接不管

	// OF和CF直接按照结果是否超过data_size位来判断
	switch (data_size)
	{
	case 8:
		{if (result == (int64_t)(int8_t)(result & 0xFF)) // 发生截断，高位全不要，低位机器数不变
		{
			cpu.eflags.CF = 0;
			cpu.eflags.OF = 0;
		}
		else
		{
			cpu.eflags.CF = 1;
			cpu.eflags.OF = 1;
		}
		break;}
	case 16:
		{if (result == (int64_t)(int16_t)(result & 0xFFFF))
		{
			cpu.eflags.CF = 0;
			cpu.eflags.OF = 0;
		}
		else
		{
			cpu.eflags.CF = 1;
			cpu.eflags.OF = 1;
		}
		break;}
	case 32:
		{if (result == (int64_t)(int32_t)(result & 0xFFFFFFFF))
		{
			cpu.eflags.CF = 0;
			cpu.eflags.OF = 0;
		}
		else
		{
			cpu.eflags.CF = 1;
			cpu.eflags.OF = 1;
		}
		break;}
	
	default:
		break;
	}
	return result;
#endif
}

// result ← DEST / SRC; remainder ← DEST % SRC;
// no eflags modified
// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	uint32_t result = 0;

	// optimization for 0 and 1
	// operand has 0
	if (src ==0)
	{
		printf("Divided by 0\n");
		fflush(stdout);
		assert(0);
		return 0;
	}
	if (dest == 0)
	{
		return 0;
	}
	// operand has 1
	if (src == 1)
	{
		return dest;
	}

	// result
	result = (uint32_t)(dest & (0xFFFFFFFFFFFFFFFF >> (64 - data_size))) / (src & (0xFFFFFFFFFFFFFFFF >> (64 - data_size)));
	return result;

#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// 下面的转换仅适合32位及一下；实际上考虑到手册中说仅关心两个64位整数相除得到一个32位整数的问题，因此这里不对传入的src和dest进行任何处理
	// src = (int64_t)sign_ext(src, data_size);
	// dest = (int64_t)sign_ext(dest, data_size);

	// optimization for 0 and 1
	// operand has 0
	if (src ==0)
	{
		printf("Divided by 0\n");
		fflush(stdout);
		assert(0);
		return 0;
	}
	if (dest == 0)
	{
		return 0;
	}
	// operand has 1
	if (src == 1)
	{
		return dest;
	}

	// result
	int32_t result = dest / src;
	return result;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	uint32_t result = 0;
	if (src == 0)
	{
		printf("Divided by 0\n");
		fflush(stdout);
		assert(0);
		return 0;
	}
	if (dest == 0)
	{
		return 0;
	}
	if (src == 1)
	{
		return 0;
	}

	result = dest % src;
	return result;

#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	if (src == 0)
	{
		printf("Divided by 0\n");
		fflush(stdout);
		assert(0);
		return 0;
	}
	if (dest == 0)
	{
		return 0;
	}
	if (src == 1)
	{
		return 0;
	}

	int32_t result = dest % src;
	return result;
#endif
}

//////////////////////////////////////////////////////////////////////////
// DEST ← DEST AND SRC;
// CF ← 0;
// OF ← 0;
// SF/ZF/PF Modified; CF/OF are set zero
uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// result
	uint32_t result = (dest & (0xFFFFFFFF >> (32 - data_size))) & (src & (0xFFFFFFFF >> (32 - data_size)));
	// change eflags
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(result);
	set_ZF(result, data_size);
	set_SF(result, data_size);

	return result;
#endif
}

// DEST ← LeftSRC XOR RightSRC
// CF ← 0
// OF ← 0
// SF/ZF/PF Modified; CF/OF are set zero; AF is undefined
uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// result
	uint32_t result = (dest & (0xFFFFFFFF >> (32 - data_size))) ^ (src & (0xFFFFFFFF >> (32 - data_size)));

	// change eflags
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(result);
	set_ZF(result, data_size);
	set_SF(result, data_size);

	return result;
#endif
}

// DEST ← DEST OR SRC;
// CF ← 0;
// OF ← 0
// SF/ZF/PF Modified; CF/OF are set zero
uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// result
	uint32_t result = (dest & (0xFFFFFFFF >> (32 - data_size))) | (src & (0xFFFFFFFF >> (32 - data_size)));
	// change eflags
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(result);
	set_ZF(result, data_size);
	set_SF(result, data_size);

	return result;
#endif
}

//////////////////////////////////////////////////////////////////////////
// 返回将 dest 算术左移 src 位后的结果， data_size 用于指明操作数长度（比特数）
// SF/ZF/PF/CF/OF modified (OF could be ignored)
// 特别说明：针对上面四个移位操作，约定只影响 `dest`操作数的低 `data_size`位，而不影响其高 `32 - data_size`位。标志位的设置根据结果的低 `data_size`位来设置。

// dest逻辑左移（补零）src位
// CF ← 最后一次移出dest的位（也就是高位）
uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;
	
	// specially for OF
	// For left shifts, the OF flag is set to the exclusive OR of the CF bit (after the shift) and the most-significant bit (top two bits) of the result.
	if (src == 1)
	{
		// get the CF
		int CF = (dest >> (data_size - 1)) & 0x00000001;
		// get the top
		int top = (dest >> (data_size - 2)) & 0x00000001;
	
		cpu.eflags.OF = (CF != top);
	}

	// result
	// to save the high 32 - datasize bits
	// attention: C语言中,当移位操作数超过了其数据类型的位数时,行为是未定义的。
	// 根据实际检验情况，右移32位时候，结果竟然变成了全1！！！！

	// test code
	// dest = sign_ext(dest, data_size);
	// printf("dest: 0x%x\n", dest);
	// printf("data_size: 0x%x\n", data_size);
	// printf("src: 0x%x\n", src);

	uint32_t high_32_datasize_dest = dest & ((uint32_t)0xFFFFFFFF << data_size);
	if (data_size == 32)
		high_32_datasize_dest = 0;
	// only save the low datasize bits, to be calculated
	uint32_t low_detasize_result = 0;

	// optimization for src > data_size
	if (src > data_size)
	{
		// low_detasize_result = 0;
		cpu.eflags.CF = 0;
		cpu.eflags.PF = 1;
		cpu.eflags.ZF = 1;
		cpu.eflags.SF = 0;
		// return (high_32_datasize_dest + low_detasize_result);
		return (high_32_datasize_dest + low_detasize_result) & (0xFFFFFFFF >> (32 - data_size));
		// 实际只返回截断后低date_size位？？？
	}
	else
	{
		low_detasize_result = ((dest & (0xFFFFFFFF >> (32 - data_size))) << src) & (0xFFFFFFFF >> (32 - data_size));
		// change eflags
		cpu.eflags.CF = (dest >> (data_size - src + 1 - 1)) & 0x00000001;
		set_PF(low_detasize_result);
		set_ZF(low_detasize_result, data_size);
		set_SF(low_detasize_result, data_size);
		// return (high_32_datasize_dest + low_detasize_result);

		// test code
		// printf("high_32_datasize_dest: 0x%x\n", high_32_datasize_dest);
		// printf("low_detasize_result: 0x%x\n", low_detasize_result);

		return (high_32_datasize_dest + low_detasize_result) & (0xFFFFFFFF >> (32 - data_size));
		// 实际只返回截断后低date_size位？？？
	}

#endif
}

// dest算术左移（补1或补0）src位
// CF ← 最后一次移出dest的位（也就是高位）
uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// specially for OF
	// same to shl
	if (src == 1)
	{
		// get the CF
		int CF = (dest >> (data_size - 1)) & 0x00000001;
		// get the top
		int top = (dest >> (data_size - 2)) & 0x00000001;
	
		cpu.eflags.OF = (CF != top);
	}

	// result
	// to save the high 32 - datasize bits
	uint32_t high_32_datasize_dest = dest & (0xFFFFFFFF << data_size);
	if (data_size == 32)
		high_32_datasize_dest = 0;
	// only save the low datasize bits, to be calculated
	uint32_t low_detasize_result = 0;

	// optimization for src > data_size
	if (src > data_size)
	{
		// set low detasize bits according to sign
		if ((dest >> (data_size - 1)) & 0x00000001)
		{
			low_detasize_result = 0xFFFFFFFF >> (32 - data_size);
			cpu.eflags.CF = 1;
			cpu.eflags.PF = 1;
			cpu.eflags.ZF = 0;
			cpu.eflags.SF = 1;
		}
		else
		{
			low_detasize_result = 0;
			cpu.eflags.CF = 0;
			cpu.eflags.PF = 1;
			cpu.eflags.ZF = 1;
			cpu.eflags.SF = 0;
		}
		
		// return (high_32_datasize_dest + low_detasize_result);
		return (high_32_datasize_dest + low_detasize_result) & (0xFFFFFFFF >> (32 - data_size));
		// 实际只返回截断后低date_size位？？？
	}
	else
	{
		// set low detasize bits according to sign
		// if ((dest >> (data_size - 1)) & 0x00000001)
		// {
		// 	// 左移低位都补0！无论算术 or 逻辑！！！！
		// 	// 补位为1（this is wrong！）
		// 	low_detasize_result = ( ((dest & (0xFFFFFFFF >> (32 - data_size))) << src) | (0xFFFFFFFF >> (32 - src)) ) & (0xFFFFFFFF >> (32 - data_size));
		// 	set_PF(low_detasize_result);
		// 	set_ZF(low_detasize_result, data_size);
		// 	set_SF(low_detasize_result, data_size);
		// 	cpu.eflags.CF = dest >> (data_size - src) & 0x00000001;
		// }
		// else
		// {
			// 补位为0
			low_detasize_result = ((dest & (0xFFFFFFFF >> (32 - data_size))) << src) & (0xFFFFFFFF >> (32 - data_size));
			set_PF(low_detasize_result);
			set_ZF(low_detasize_result, data_size);
			set_SF(low_detasize_result, data_size);
			cpu.eflags.CF = dest >> (data_size - src) & 0x00000001;
		// }
		// return (high_32_datasize_dest + low_detasize_result);
		return (high_32_datasize_dest + low_detasize_result) & (0xFFFFFFFF >> (32 - data_size));
		// 实际只返回截断后低date_size位？？？
	}

#endif
}

// dest逻辑右移src位
// CF ← 最后一次移出dest的位（也就是低位）
uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// specially for OF
	// For right shifts, the OF flag is set to the high order of the original operand.
	if (src == 1)
	{
		// get the OF
		cpu.eflags.OF = (dest >> (data_size - 1)) & 0x00000001;
	}

	// to save the high 32 - datasize bits
	uint32_t high_32_datasize_dest = dest & (0xFFFFFFFF << data_size);
	if (data_size == 32)
		high_32_datasize_dest = 0;
	// only save the low datasize bits, to be calculated
	uint32_t low_detasize_result = 0;

	// optimization for src > data_size
	if (src > data_size)
	{
		// low_detasize_result = 0;
		cpu.eflags.CF = 0;
		cpu.eflags.PF = 1;
		cpu.eflags.ZF = 1;
		cpu.eflags.SF = 0;
		// return (high_32_datasize_dest + low_detasize_result);
		return (high_32_datasize_dest + low_detasize_result) & (0xFFFFFFFF >> (32 - data_size));
		// 实际只返回截断后低date_size位？？？
	}
	else
	{
		low_detasize_result = ((dest & (0xFFFFFFFF >> (32 - data_size))) >> src) & (0xFFFFFFFF >> (32 - data_size));
		// change eflags
		cpu.eflags.CF = (dest >> (src - 1)) & 0x00000001;
		set_PF(low_detasize_result);
		set_ZF(low_detasize_result, data_size);
		set_SF(low_detasize_result, data_size);
		// return (high_32_datasize_dest + low_detasize_result);
		return (high_32_datasize_dest + low_detasize_result) & (0xFFFFFFFF >> (32 - data_size));
		// 实际只返回截断后低date_size位？？？
	}
#endif
}

// dest算术右移src位
// CF ← 最后一次移出dest的位（也就是低位）
uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	// printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// return 0;

	// specially for OF
	// For sar, the OF flag is cleared.
	if (src == 1)
	{
		// get the OF
		cpu.eflags.OF = 0;
	}

	// to save the high 32 - datasize bits
	uint32_t high_32_datasize_dest = dest & (0xFFFFFFFF << data_size);
	if (data_size == 32)
		high_32_datasize_dest = 0;
	// only save the low datasize bits, to be calculated
	uint32_t low_detasize_result = 0;

	// optimization for src > data_size
	if (src > data_size)
	{
		// set low detasize bits according to sign
		if ((dest >> (data_size - 1)) & 0x00000001)
		{
			low_detasize_result = 0xFFFFFFFF >> (32 - data_size);
			cpu.eflags.CF = 1;
			cpu.eflags.PF = 1;
			cpu.eflags.ZF = 0;
			cpu.eflags.SF = 1;
		}
		else
		{
			low_detasize_result = 0;
			cpu.eflags.CF = 0;
			cpu.eflags.PF = 1;
			cpu.eflags.ZF = 1;
			cpu.eflags.SF = 0;
		}
		
		// return (high_32_datasize_dest + low_detasize_result);
		return (high_32_datasize_dest + low_detasize_result) & (0xFFFFFFFF >> (32 - data_size));
		// 实际只返回截断后低date_size位？？？
	}
	else
	{
		// set low detasize bits according to sign
		if ((dest >> (data_size - 1)) & 0x00000001)
		{
			// 补位为1，先截断，再右移，再把data_size - src + 1 到 data_size位补1，最后再截断
			low_detasize_result = (((dest & (0xFFFFFFFF >> (32 - data_size))) >> src) | ((0xFFFFFFFF >> (32 - src)) << (data_size - src))) & (0xFFFFFFFF >> (32 - data_size));
			set_PF(low_detasize_result);
			set_ZF(low_detasize_result, data_size);
			set_SF(low_detasize_result, data_size);
			cpu.eflags.CF = dest >> (src - 1) & 0x00000001;
		}
		else
		{
			// 补位为0
			low_detasize_result = ((dest & (0xFFFFFFFF >> (32 - data_size))) >> src) & (0xFFFFFFFF >> (32 - data_size));
			set_PF(low_detasize_result);
			set_ZF(low_detasize_result, data_size);
			set_SF(low_detasize_result, data_size);
			cpu.eflags.CF = dest >> (src - 1) & 0x00000001;
		}
		// return (high_32_datasize_dest + low_detasize_result);
		return (high_32_datasize_dest + low_detasize_result) & (0xFFFFFFFF >> (32 - data_size));
		// 实际只返回截断后低date_size位？？？
	}
#endif
}


