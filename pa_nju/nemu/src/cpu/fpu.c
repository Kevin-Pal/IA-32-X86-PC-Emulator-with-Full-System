#include "nemu.h"
#include "cpu/fpu.h"

FPU fpu;
// special values
FLOAT p_zero, n_zero, p_inf, n_inf, p_nan, n_nan;

// the last three bits of the significand are reserved for the GRS bits
// it is of great significance to clearly define the meaning of the parameter exp and sig_grs
// for exp, when exp=0, it means the real exp is 2^-126, otherwise, it means 2^(exp-127) (this includes negative exp!!!)
// for sig_res, the point is between the 26rd and 27th bit, and the GRS bits are the last three bit
// so the real value is (-1)^sign * 2^exp_real(0 or exp-127) * sig_res(point in the 26rd bit) 
inline uint32_t internal_normalize(uint32_t sign, int32_t exp, uint64_t sig_grs)
{

	uint32_t sticky = 0;
	// normalization
	bool overflow = false; // true if the result is INFINITY or 0 during normalize

	// need to be right-normalized
	if ((sig_grs >> (23 + 3)) > 1 || exp < 0)
	{
		// normalize toward right
		while ((((sig_grs >> (23 + 3)) > 1) && exp < 0xff) // condition 1
			   ||										   // or
			   (sig_grs > 0x04 && exp < 0)				   // condition 2
			   // 一直到0101为止，看起来0101末位已经是0，但靠着GRS还是能归化成1的
			   )
		{
			/* TODO: shift right, pay attention to sticky bit*/
			// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
			// fflush(stdout);
			// assert(0);
			sticky = sticky | (sig_grs & 0x1);
			sig_grs = sig_grs >> 1;
			// set the sticky bit
			// when shift happens, the sticky bit is change and must be set again
			sig_grs |= sticky;
			exp++;
		}

		if (exp >= 0xff)
		{
			/* TODO: assign the number to infinity */
			// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
			// fflush(stdout);
			// assert(0);
			// overflow = true;
			return sign ? N_INF_F : P_INF_F;
		}
		if (exp == 0) // this could happen when doing add and sub
		// 只在exp为负数右规时发生
		{
			// we have a denormal here, the exponent is 0, but means 2^-126,
			// as a result, the significand should shift right once more
			/* TODO: shift right, pay attention to sticky bit*/
			// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
			// fflush(stdout);
			// assert(0);
			sticky = sig_grs & 0x1;
			sig_grs = sig_grs >> 1;
			sig_grs |= sticky;
		}
		if (exp < 0)
		{
			/* TODO: assign the number to zero */
			// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
			// fflush(stdout);
			// assert(0);
			// overflow = true;
			return sign ? N_ZERO_F : P_ZERO_F;
		}
	}

	// need to be left-normalized
	else if (((sig_grs >> (23 + 3)) == 0) && exp > 0)
	{
		// normalize toward left
		while (((sig_grs >> (23 + 3)) == 0) && exp > 0)
		{
			/* TODO: shift left */
			// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
			// fflush(stdout);
			// assert(0);

			sig_grs = sig_grs << 1;
			// What about sticky bit?
			exp--;
		}
		if (exp == 0)
		{
			// denormal
			/* TODO: shift right, pay attention to sticky bit*/
			// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
			// fflush(stdout);
			// assert(0);

			// It seams that there is no need to set the sticky bit here, as the last bit is 0
			sticky = sig_grs & 0x1;
			sig_grs = sig_grs >> 1;
			sig_grs |= sticky;
		}
	}
	else if (exp == 0 && sig_grs >> (23 + 3) == 1)
	{
		// two denormals result in a normal
		exp++;
	}

	if (!overflow)
	{
		// /* TODO: round up and remove the GRS bits */
		// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
		// fflush(stdout);
		// assert(0);
		uint32_t GRS = sig_grs & 0x7;
		sig_grs = sig_grs >> 3;
		if (GRS > 4)
		{
			sig_grs = sig_grs + 1;
			// 进位，进位后可能要再次右规
			if ((sig_grs >> 23) > 1)
			{
				exp++;
				// be caeful of the overflow case
				if (exp >= 0xff)
				{
					return sign ? N_INF_F : P_INF_F;
				}
				sig_grs = sig_grs >> 1;
			}
		}
		else if(GRS == 4)
		{
			// round to even
			if(sig_grs & 0x1)
			{
				sig_grs = sig_grs + 1;
				if ((sig_grs >> 23) > 1)
				{
					exp++;
					// be caeful of the overflow case
					if (exp >= 0xff)
					{
						return sign ? N_INF_F : P_INF_F;
					}
					sig_grs = sig_grs >> 1;
				}
			}
		}
		else
		{
			// do nothing
		}	
		// keep the lowest 23 bits
		sig_grs = sig_grs & 0x7fffff;
	}

	FLOAT f;
	f.sign = sign;
	f.exponent = (uint32_t)(exp & 0xff);
	f.fraction = sig_grs; // here only the lowest 23 bits are kept
	return f.val;
}

CORNER_CASE_RULE corner_add[] = {
	{P_ZERO_F, P_ZERO_F, P_ZERO_F},
	{N_ZERO_F, P_ZERO_F, P_ZERO_F},
	{P_ZERO_F, N_ZERO_F, P_ZERO_F},
	{N_ZERO_F, N_ZERO_F, N_ZERO_F},
	{P_INF_F, N_INF_F, N_NAN_F},
	{N_INF_F, P_INF_F, N_NAN_F},
	{P_INF_F, P_NAN_F, P_NAN_F},
	{P_INF_F, N_NAN_F, N_NAN_F},
	{N_INF_F, P_NAN_F, P_NAN_F},
	{N_INF_F, N_NAN_F, N_NAN_F},
	{N_NAN_F, P_NAN_F, P_NAN_F},
};

// a + b
uint32_t internal_float_add(uint32_t b, uint32_t a)
{

	// corner cases
	int i = 0;
	for (; i < sizeof(corner_add) / sizeof(CORNER_CASE_RULE); i++)
	{
		if (a == corner_add[i].a && b == corner_add[i].b)
			return corner_add[i].res;
	}
	if (a == P_ZERO_F || a == N_ZERO_F)
	{
		return b;
	}
	if (b == P_ZERO_F || b == N_ZERO_F)
	{
		return a;
	}

	FLOAT f, fa, fb;
	fa.val = a;
	fb.val = b;
	// infity, NaN
	if (fa.exponent == 0xff)
	{
		return a;
	}
	if (fb.exponent == 0xff)
	{
		return b;
	}

	// this means a is always the smaller one
	if (fa.exponent > fb.exponent)
	{
		fa.val = b;
		fb.val = a;
	}

	uint32_t sig_a, sig_b, sig_res;
	sig_a = fa.fraction;
	if (fa.exponent != 0)
		sig_a |= 0x800000; // the hidden 1
	sig_b = fb.fraction;
	if (fb.exponent != 0)
		sig_b |= 0x800000; // the hidden 1

	// alignment shift for fa
	uint32_t shift = 0;

	/* TODO: shift = ? */
	// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// assert(shift >= 0);
	shift = (fb.exponent==0 ? fb.exponent+1 : fb.exponent) - (fa.exponent==0 ? fa.exponent+1 : fa.exponent);

	sig_a = (sig_a << 3); // guard, round, sticky
	sig_b = (sig_b << 3);

	uint32_t sticky = 0;
	while (shift > 0)
	{
		// sticky bit is the last bit of sig_a, and it is 1 if any bit in the lower part is 1
		sticky = sticky | (sig_a & 0x1);
		sig_a = sig_a >> 1;
		// set the sticky bit
		// when shift happens, the sticky bit is change and must be set again
		sig_a |= sticky;
		shift--;
	}

	// fraction add
	if (fa.sign)
	{
		sig_a *= -1;
	}
	if (fb.sign)
	{
		sig_b *= -1;
	}

	sig_res = sig_a + sig_b;

	if (sign(sig_res))
	{
		f.sign = 1;
		sig_res *= -1;
	}
	else
	{
		f.sign = 0;
	}

	uint32_t exp_res = fb.exponent;
	return internal_normalize(f.sign, exp_res, sig_res);
}

CORNER_CASE_RULE corner_sub[] = {
	{P_ZERO_F, P_ZERO_F, P_ZERO_F},
	{N_ZERO_F, P_ZERO_F, N_ZERO_F},
	{P_ZERO_F, N_ZERO_F, P_ZERO_F},
	{N_ZERO_F, N_ZERO_F, P_ZERO_F},
	{P_NAN_F, P_NAN_F, P_NAN_F},
	{N_NAN_F, P_NAN_F, P_NAN_F},
	{P_NAN_F, N_NAN_F, P_NAN_F},
	{N_NAN_F, N_NAN_F, N_NAN_F},
};

// a - b
uint32_t internal_float_sub(uint32_t b, uint32_t a)
{
	// change the sign of b
	int i = 0;
	for (; i < sizeof(corner_sub) / sizeof(CORNER_CASE_RULE); i++)
	{
		if (a == corner_sub[i].a && b == corner_sub[i].b)
			return corner_sub[i].res;
	}
	if (a == P_NAN_F || a == N_NAN_F)
		return a;
	if (b == P_NAN_F || b == N_NAN_F)
		return b;
	FLOAT fb;
	fb.val = b;
	fb.sign = ~fb.sign;
	return internal_float_add(fb.val, a);
}

CORNER_CASE_RULE corner_mul[] = {
	{P_ZERO_F, P_INF_F, N_NAN_F},
	{P_ZERO_F, N_INF_F, N_NAN_F},
	{N_ZERO_F, P_INF_F, N_NAN_F},
	{N_ZERO_F, N_INF_F, N_NAN_F},
	{P_INF_F, P_ZERO_F, N_NAN_F},
	{P_INF_F, N_ZERO_F, N_NAN_F},
	{N_INF_F, P_ZERO_F, N_NAN_F},
	{N_INF_F, N_ZERO_F, N_NAN_F},
};

// a * b
uint32_t internal_float_mul(uint32_t b, uint32_t a)
{
	int i = 0;
	for (; i < sizeof(corner_mul) / sizeof(CORNER_CASE_RULE); i++)
	{
		if (a == corner_mul[i].a && b == corner_mul[i].b)
			return corner_mul[i].res;
	}

	if (a == P_NAN_F || a == N_NAN_F || b == P_NAN_F || b == N_NAN_F)
		return a == P_NAN_F || b == P_NAN_F ? P_NAN_F : N_NAN_F;

	FLOAT fa, fb, f;
	fa.val = a;
	fb.val = b;
	f.sign = fa.sign ^ fb.sign;

	if (a == P_ZERO_F || a == N_ZERO_F)
		return fa.sign ^ fb.sign ? N_ZERO_F : P_ZERO_F;
	if (b == P_ZERO_F || b == N_ZERO_F)
		return fa.sign ^ fb.sign ? N_ZERO_F : P_ZERO_F;
	if (a == P_INF_F || a == N_INF_F)
		return fa.sign ^ fb.sign ? N_INF_F : P_INF_F;
	if (b == P_INF_F || b == N_INF_F)
		return fa.sign ^ fb.sign ? N_INF_F : P_INF_F;

	uint64_t sig_a, sig_b, sig_res;
	sig_a = fa.fraction;
	if (fa.exponent != 0)
		sig_a |= 0x800000; // the hidden 1
	sig_b = fb.fraction;
	if (fb.exponent != 0)
		sig_b |= 0x800000; // the hidden 1

	// exp=0 and exp=1 means the same thing: 2^-126
	// the only diferece is that the hidden bit is 0 or 1
	// as we have already set the hidden bit according to the exp, we can just set the exp to 1
	// this has a benefit that we can avoid the special case of exp=0, every exp substracting 127 is the real exp
	if (fa.exponent == 0)
		fa.exponent++;
	if (fb.exponent == 0)
		fb.exponent++;

	sig_res = sig_a * sig_b; // 24b * 24b
	uint32_t exp_res = 0;

	/* TODO: exp_res = ? leave space for GRS bits. */
	// printf("\e[0;31mPlease implement me at fpu.c\e[0m\n");
	// fflush(stdout);
	// assert(0);
	// realexp = (exp_a-127) + (exp_b-127) is the real exp
	// and considering .23 * .23 = .46,
	// if we still consider that the point is between the 23nd and 24th bit, the exp should minus 23 
	// so realexp = (exp_a-127) + (exp_b-127) - 23
	// as for exp_res, accoring to the definition of "internal_normalize", 
	// the real exp is exp_res-127 when exp_res != 0, and the real exp is -126 when exp_res = 0
	// so exp_res = realexp + 127 = (exp_a-127) + (exp_b-127) - 23 + 127 = exp_a + exp_b - 127 - 23
	// should be careful for the corner case
	// when exp_a + exp_b - 127 - 23 = 0, the real exp is -127, but the exp_res should be 0, indicates real exp is -126
	// when exp_a + exp_b - 127 - 23 ≠ 0, the real exp is (exp_a-127) + (exp_b-127) - 23, the exp_res should be exp_a + exp_b - 127 - 23, indicates real exp is exp_a + exp_b - 127 - 23 -127, the result is the same

	// finally need to add 3 for the GRS bits
	// so realexp = (exp_a-127) + (exp_b-127) - 23 + 3

	 exp_res = fa.exponent + fb.exponent - 127 - 23 + 3;
	//  uint32_t sticky = 0;
	//  if (exp_res == 0) // indicates real exp is -126, while the real exp is actually -127
	//  {
	// 	// sigres shift right once
	// 	sticky = sig_res & 0x1;
	// 	sig_res = sig_res >> 1;
	// 	// sticky bits
	// 	sig_res |= sticky;
	//  }

	// but the above doesn't work, why?
 
	return internal_normalize(f.sign, exp_res, sig_res);
}

CORNER_CASE_RULE corner_div[] = {
	{P_ZERO_F, P_ZERO_F, N_NAN_F},
	{N_ZERO_F, P_ZERO_F, N_NAN_F},
	{P_ZERO_F, N_ZERO_F, N_NAN_F},
	{N_ZERO_F, N_ZERO_F, N_NAN_F},
	{P_INF_F, P_ZERO_F, P_INF_F},
	{N_INF_F, P_ZERO_F, N_INF_F},
	{P_INF_F, N_ZERO_F, N_INF_F},
	{N_INF_F, N_ZERO_F, P_INF_F},
	{P_INF_F, P_INF_F, N_NAN_F},
	{N_INF_F, P_INF_F, N_NAN_F},
	{P_INF_F, N_INF_F, N_NAN_F},
	{N_INF_F, N_INF_F, N_NAN_F},
};
// a / b
uint32_t internal_float_div(uint32_t b, uint32_t a)
{

	int i = 0;
	for (; i < sizeof(corner_div) / sizeof(CORNER_CASE_RULE); i++)
	{
		if (a == corner_div[i].a && b == corner_div[i].b)
			return corner_div[i].res;
	}

	FLOAT f, fa, fb;
	fa.val = a;
	fb.val = b;

	if (a == P_NAN_F || a == N_NAN_F || b == P_NAN_F || b == N_NAN_F)
		return a == P_NAN_F || b == P_NAN_F ? P_NAN_F : N_NAN_F;
	if (a == P_INF_F || a == N_INF_F)
	{
		return fa.sign ^ fb.sign ? N_INF_F : P_INF_F;
	}
	if (b == P_ZERO_F || b == N_ZERO_F)
	{
		return fa.sign ^ fb.sign ? N_INF_F : P_INF_F;
	}
	if (a == P_ZERO_F || a == N_ZERO_F)
	{
		fa.sign = fa.sign ^ fb.sign;
		return fa.val;
	}
	if (b == P_INF_F || b == N_INF_F)
	{
		return fa.sign ^ fb.sign ? N_ZERO_F : P_ZERO_F;
	}

	f.sign = fa.sign ^ fb.sign;

	uint64_t sig_a, sig_b, sig_res;
	sig_a = fa.fraction;
	if (fa.exponent != 0)
		sig_a |= 0x800000; // the hidden 1
	sig_b = fb.fraction;
	if (fb.exponent != 0)
		sig_b |= 0x800000; // the hidden 1

	// efforts to maintain the precision of the result
	int shift = 0;
	while (sig_a >> 63 == 0)
	{
		sig_a <<= 1;
		shift++;
	}
	while ((sig_b & 0x1) == 0)
	{
		sig_b >>= 1;
		shift++;
	}

	sig_res = sig_a / sig_b;

	if (fa.exponent == 0)
		fa.exponent++;
	if (fb.exponent == 0)
		fb.exponent++;
	uint32_t exp_res = fa.exponent - fb.exponent + 127 - (shift - 23 - 3);
	return internal_normalize(f.sign, exp_res, sig_res);
}

void fpu_load(uint32_t val)
{
	fpu.status.top = fpu.status.top == 0 ? 7 : fpu.status.top - 1;
	fpu.regStack[fpu.status.top].val = val;
}

uint32_t fpu_store()
{
	uint32_t val = fpu.regStack[fpu.status.top].val;
	fpu.status.top = (fpu.status.top + 1) % 8;
	return val;
}

uint32_t fpu_peek()
{
	uint32_t val = fpu.regStack[fpu.status.top].val;
	return val;
}

void fpu_add(uint32_t val)
{
	/*
	float *a = (float*)&fpu.regStack[fpu.status.top].val;
	float *b = (float*)&val;
	float c = *a + *b;
	uint32_t *d = (uint32_t *)&c;
	fpu.regStack[fpu.status.top].val = *d;
	*/
	fpu.regStack[fpu.status.top].val = internal_float_add(val, fpu.regStack[fpu.status.top].val);
}

void fpu_add_idx(uint32_t idx, uint32_t store_idx)
{
	/*
	float *a = (float*)&fpu.regStack[fpu.status.top].val;
	float *b = (float*)&fpu.regStack[(fpu.status.top + idx) % 8].val;
	float c = *a + *b;
	uint32_t *d = (uint32_t *)&c;
	fpu.regStack[(fpu.status.top + store_idx) % 8].val = *d;
	*/
	uint32_t a = fpu.regStack[fpu.status.top].val;
	uint32_t b = fpu.regStack[(fpu.status.top + idx) % 8].val;
	fpu.regStack[(fpu.status.top + store_idx) % 8].val = internal_float_add(b, a);
}

void fpu_sub(uint32_t val)
{
	/*
	float *a = (float*)&fpu.regStack[fpu.status.top].val;
	float *b = (float*)&val;
	float c = *a - *b;
	//printf("f %f - %f = %f\n", *a, *b, c);
	uint32_t *d = (uint32_t *)&c;
	fpu.regStack[fpu.status.top].val = *d;
	*/
	fpu.regStack[fpu.status.top].val = internal_float_sub(val, fpu.regStack[fpu.status.top].val);
}

void fpu_mul(uint32_t val)
{
	/*
	float *a = (float*)&fpu.regStack[fpu.status.top].val;
	float *b = (float*)&val;
	float c = *a * *b;
	uint32_t *d = (uint32_t *)&c;
	fpu.regStack[fpu.status.top].val = *d;
	*/
	fpu.regStack[fpu.status.top].val = internal_float_mul(val, fpu.regStack[fpu.status.top].val);
}

void fpu_mul_idx(uint32_t idx, uint32_t store_idx)
{
	/*
	float *a = (float*)&fpu.regStack[fpu.status.top].val;
	float *b = (float*)&fpu.regStack[(fpu.status.top + idx) % 8].val;
	float c = *a * *b;
	uint32_t *d = (uint32_t *)&c;
	fpu.regStack[(fpu.status.top + store_idx) % 8].val = *d;
	*/
	uint32_t a = fpu.regStack[fpu.status.top].val;
	uint32_t b = fpu.regStack[(fpu.status.top + idx) % 8].val;
	fpu.regStack[(fpu.status.top + store_idx) % 8].val = internal_float_mul(b, a);
}

void fpu_div(uint32_t val)
{
	/*
	float *a = (float*)&fpu.regStack[fpu.status.top].val;
	float *b = (float*)&val;
	float c = *a / *b;
	// printf("f %f / %f = %f\n", *a, *b, c);
	uint32_t *d = (uint32_t *)&c;
	fpu.regStack[fpu.status.top].val = *d;
	*/
	fpu.regStack[fpu.status.top].val = internal_float_div(val, fpu.regStack[fpu.status.top].val);
}

void fpu_xch(uint32_t idx)
{
	idx = (fpu.status.top + idx) % 8;
	uint32_t temp = fpu.regStack[fpu.status.top].val;
	fpu.regStack[fpu.status.top].val = fpu.regStack[idx].val;
	fpu.regStack[idx].val = temp;
}

void fpu_copy(uint32_t idx)
{
	idx = (fpu.status.top + idx) % 8;
	fpu.regStack[idx].val = fpu.regStack[fpu.status.top].val;
}

void fpu_cmp(uint32_t idx)
{
	idx = (fpu.status.top + idx) % 8;
	float *a = (float *)&fpu.regStack[fpu.status.top].val;
	float *b = (float *)&fpu.regStack[idx].val;
	if (*a > *b)
	{
		fpu.status.c0 = fpu.status.c2 = fpu.status.c3 = 0;
		//printf("f %f > %f\n", *a, *b);
		//printf("f %x > %x\n", *((uint32_t *)a), *((uint32_t *)b));
	}
	else if (*a < *b)
	{
		fpu.status.c0 = 1;
		fpu.status.c2 = fpu.status.c3 = 0;
		//printf("f %f < %f\n", *a, *b);
	}
	else
	{
		fpu.status.c0 = fpu.status.c2 = 0;
		fpu.status.c3 = 1;
		//printf("f %f == %f\n", *a, *b);
	}
}

void fpu_cmpi(uint32_t idx)
{
	idx = (fpu.status.top + idx) % 8;
	float *a = (float *)&fpu.regStack[fpu.status.top].val;
	float *b = (float *)&fpu.regStack[idx].val;
	if (*a > *b)
	{
		cpu.eflags.ZF = cpu.eflags.PF = cpu.eflags.CF = 0;
	}
	else if (*a < *b)
	{
		cpu.eflags.ZF = cpu.eflags.PF = 0;
		cpu.eflags.CF = 1;
	}
	else
	{
		cpu.eflags.CF = cpu.eflags.PF = 0;
		cpu.eflags.ZF = 1;
	}
}
