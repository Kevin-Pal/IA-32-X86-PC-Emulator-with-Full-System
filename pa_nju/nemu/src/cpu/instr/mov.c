#include "cpu/instr.h"

static void instr_execute_2op() 
{
	operand_read(&opr_src);
	opr_dest.val = opr_src.val;
	operand_write(&opr_dest);
}

make_instr_impl_2op(mov, r, rm, b)
make_instr_impl_2op(mov, r, rm, v)
make_instr_impl_2op(mov, rm, r, b)
make_instr_impl_2op(mov, rm, r, v)
make_instr_impl_2op(mov, i, rm, b)
make_instr_impl_2op(mov, i, rm, v)
make_instr_impl_2op(mov, i, r, b)
make_instr_impl_2op(mov, i, r, v)
make_instr_impl_2op(mov, a, o, b)
make_instr_impl_2op(mov, a, o, v)
make_instr_impl_2op(mov, o, a, b)
make_instr_impl_2op(mov, o, a, v)

make_instr_func(mov_zrm82r_v) {
	int len = 1;
	OPERAND r, rm;
	r.data_size = data_size;
	rm.data_size = 8;
	len += modrm_r_rm(eip + 1, &r, &rm);
	
	operand_read(&rm);
	r.val = rm.val;
	operand_write(&r);

	print_asm_2("mov", "", len, &rm, &r);
	return len;
}

make_instr_func(mov_zrm162r_l) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = 32;
        rm.data_size = 16;
        len += modrm_r_rm(eip + 1, &r, &rm);

        operand_read(&rm);
        r.val = rm.val;
        operand_write(&r);
	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

make_instr_func(mov_srm82r_v) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = data_size;
        rm.data_size = 8;
        len += modrm_r_rm(eip + 1, &r, &rm);
        
	operand_read(&rm);
        r.val = sign_ext(rm.val, 8);
        operand_write(&r);
	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

make_instr_func(mov_srm162r_l) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = 32;
        rm.data_size = 16;
        len += modrm_r_rm(eip + 1, &r, &rm);
        operand_read(&rm);
        r.val = sign_ext(rm.val, 16);
        operand_write(&r);

	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

// 0x0f 20
// mov cr0, r32
make_instr_func(mov_c2r_l) {
        int len = 1;
        OPERAND cr0, r32;
        cr0.data_size = 32;
        r32.data_size = 32;
        
        // 这里因为CREG比较特殊，所以不能用modrm_r_rm，需要手动解码
        MODRM modrm;
        modrm.val = instr_fetch(eip + 1, 1);
        len += 1;

        cr0.addr = modrm.reg_opcode;
        cr0.type = OPR_CREG;
        cr0.sreg = SREG_CS;
        operand_read(&cr0);

        r32.addr = modrm.rm;
        r32.type = OPR_REG;
        r32.sreg = SREG_CS;
        r32.val = cr0.val;
        operand_write(&r32);

        print_asm_2("mov", "crX", len, &r32, &cr0);
        return len;
}

// 0x0f 22
// mov r32, cr0
make_instr_func(mov_r2c_l) {
        int len = 1;
        OPERAND cr0, r32;
        cr0.data_size = 32;
        r32.data_size = 32;
        
        // 这里因为CREG比较特殊，所以不能用modrm_r_rm，需要手动解码
        MODRM modrm;
        modrm.val = instr_fetch(eip + 1, 1);
        len += 1;

        r32.addr = modrm.rm;
        r32.type = OPR_REG;
        r32.sreg = SREG_CS;
        operand_read(&r32);

        cr0.addr = modrm.reg_opcode;
        cr0.type = OPR_CREG;
        cr0.sreg = SREG_CS;
        cr0.val = r32.val;
        operand_write(&cr0);

        print_asm_2("mov", "r32", len, &cr0, &r32);
        return len;
}

// 0x8e /r
// mov rm16, sreg
make_instr_func(mov_rm2s_w) {
        int len = 1;
        OPERAND rm, sreg;
        rm.data_size = 16;
        sreg.data_size = 16;
        
        len += modrm_r_rm(eip + 1, &sreg, &rm);
        operand_read(&rm);
        
        sreg.val = rm.val;
        sreg.type = OPR_SREG;
        sreg.sreg = SREG_CS;
        operand_write(&sreg);

        load_sreg(sreg.addr); // 更新段寄存器的值
        print_asm_2("mov", "sreg", len, &rm, &sreg);
        return len;
}
