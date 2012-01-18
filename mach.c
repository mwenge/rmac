//
// RMAC - Reboot's Macro Assembler for the Atari Jaguar Console System
// MACH.C - Code Generation
// Copyright (C) 199x Landon Dyer, 2011 Reboot and Friends
// RMAC derived from MADMAC v1.07 Written by Landon Dyer, 1986
// Source Utilised with the Kind Permission of Landon Dyer
//

#include "mach.h"
#include "error.h"
#include "sect.h"
#include "direct.h"
#include "token.h"
#include "procln.h"
#include "risca.h"

#define DEF_KW
#include "kwtab.h"

// Common error messages
char * range_error = "expression out of range";
char * abs_error = "illegal absolute expression";
char * seg_error = "bad (section) expression";
char * rel_error = "illegal relative address";
char * siz_error = "bad size specified";
char * undef_error = "undefined expression";
char * fwd_error = "forward or undefined expression";

extern int ea0gen(WORD);
extern int ea1gen(WORD);

// Include code tables
MNTAB machtab[] = {
   { (WORD)-1, (unsigned long)-1L, (unsigned long)-1L, 0x0000, 0, m_badmode }, // 0 
   #include "68ktab.h"
   {  0,  0L,  0L, 0x0000, 0, m_unimp   }                   // Last entry
};

// Register number << 9
WORD reg_9[8] = {
      0, 1<<9, 2<<9, 3<<9,
   4<<9, 5<<9, 6<<9, 7<<9
};

// SIZB==>00, SIZW==>01, SIZL==>10, SIZN==>01 << 6
WORD siz_6[] = {
   (WORD)-1,                                                // n/a 
   0,                                                       // SIZB
   1<<6, (WORD)-1,                                          // SIZW, n/a 
   2<<6, (WORD)-1, (WORD)-1, (WORD)-1,                      // SIZL, n/a, n/a, n/a 
   1<<6                                                     // SIZN 
};

// Byte/word/long size for MOVE instrs
WORD siz_12[] = {
   (WORD)-1,
   0x1000,                                                  // Byte 
   0x3000, (WORD)-1,                                        // Word 
   0x2000, (WORD)-1, (WORD)-1, (WORD)-1,                    // Long
   0x3000                                                   // Word (SIZN)
};

// Word/long size (0=.w, 1=.l) in bit 8
WORD lwsiz_8[] = {
   (WORD)-1,                                                // n/a
   (WORD)-1,                                                // SIZB
   0, (WORD)-1,                                             // SIZW, n/a
   1<<8, (WORD)-1, (WORD)-1, (WORD)-1,                      // SIZL, n/a, n/a, n/a
   0                                                        // SIZN
};

// Addressing mode in bits 6..11 (register/mode fields are reversed)
WORD am_6[] = {
   00000, 01000, 02000, 03000, 04000, 05000, 06000, 07000,
   00100, 01100, 02100, 03100, 04100, 05100, 06100, 07100,
   00200, 01200, 02200, 03200, 04200, 05200, 06200, 07200,
   00300, 01300, 02300, 03300, 04300, 05300, 06300, 07300,
   00400, 01400, 02400, 03400, 04400, 05400, 06400, 07400,
   00500, 01500, 02500, 03500, 04500, 05500, 06500, 07500,
   00600, 01600, 02600, 03600, 04600, 05600, 06600, 07600,
   00700, 01700, 02700, 03700, 04700, 05700, 06700, 07700
};


// Error messages
int m_unimp(void)
{
	return (int)error("unimplemented mnemonic");
}


int m_badmode(void)
{
	return (int)error("inappropriate addressing mode");
}


int m_self(WORD inst)
{
	D_word(inst);
	return 0;
}


//
// Do one EA in bits 0..5
// 
// Bits in `inst' have the following meaning:
// 
// Bit zero specifies which ea (ea0 or ea1) to generate in the lower six bits
// of the instr.
// 
// If bit one is set, the OTHER ea (the one that wasn't generated by bit zero)
// is generated after the instruction. Regardless of bit 0's value, ea0 is
// always deposited in memory before ea1.
// 
// If bit two is set, standard size bits are set in the instr in bits 6 and 7.
// 
// If bit four is set, bit three specifies which eaXreg to place in bits 9..11
// of the instr.
//
int m_ea(WORD inst, WORD siz)
{
	WORD flg;

	flg = inst;                                              // Save flag bits 
	inst &= ~0x3f;                                           // Clobber flag bits in instr 

	if (flg & 4)                                              // Install "standard" instr size bits 
		inst |= siz_6[siz];

	if (flg & 16)
	{                                           // OR-in register number 
		if (flg & 8)
		{
			inst |= reg_9[a1reg];                              // ea1reg in bits 9..11 
		}
		else
		{
			inst |= reg_9[a0reg];                              // ea0reg in bits 9..11 
		}
	}

	if (flg & 1)
	{                                            // Use am1 
		inst |= am1 | a1reg;                                  // Get ea1 into instr 
		D_word(inst);                                         // Deposit instr 

		if (flg & 2)                                           // Generate ea0 if requested 
			ea0gen(siz);

		ea1gen(siz);                                          // Generate ea1 
	}
	else
	{                                                 // Use am0 
		inst |= am0 | a0reg;	                                 // Get ea0 into instr 
		D_word(inst);                                         // Deposit instr 
		ea0gen(siz);                                          // Generate ea0 

		if (flg & 2)                                           // Generate ea1 if requested 
			ea1gen(siz);
	}

	return 0;
}


//
// Dx,Dy nnnnXXXnssnnnYYY If bit 0 of `inst' is set, install size bits in bits
// 6..7
//
int m_abcd(WORD inst, WORD siz)
{
	if (inst & 1)
	{                                           // Install size bits 
		--inst;
		inst |= siz_6[siz];
	}

	inst |= a0reg | reg_9[a1reg];
	D_word(inst);

	return 0;
}


//
// {adda} ea,AREG
//
int m_adda(WORD inst, WORD siz)
{
	inst |= am0 | a0reg | lwsiz_8[siz] | reg_9[a1reg];
	D_word(inst);
	ea0gen(siz);                                             // Gen EA 

	return 0;
}


//
// If bit 0 of `inst' is 1, install size bits in bits 6..7 of instr.
// If bit 1 of `inst' is 1, install a1reg in bits 9..11 of instr.
// 
int m_reg(WORD inst, WORD siz)
{
	if (inst & 1)                                             // Install size bits 
		inst |= siz_6[siz];

	if (inst & 2)                                             // Install other register (9..11) 
		inst |= reg_9[a1reg];

	inst &= ~7;                                              // Clear off crufty bits 
	inst |= a0reg;                                           // Install first register 
	D_word(inst);

	return 0;
}


//
// <op> #expr
//
int m_imm(WORD inst, WORD siz)
{
	D_word(inst);
	ea0gen(siz);

	return 0;
}


//
// <op>.b #expr
//
int m_imm8(WORD inst, WORD siz)
{
	siz = siz;
	D_word(inst);
	ea0gen(SIZB);

	return 0;
}


//
// <shift> Dn,Dn
//
int m_shr(WORD inst, WORD siz)
{
	inst |= reg_9[a0reg] | a1reg | siz_6[siz];
	D_word(inst);

	return 0;
}


//
// <shift> #n,Dn
//
int m_shi(WORD inst, WORD siz)
{
	inst |= a1reg | siz_6[siz];

	if (a0exattr & DEFINED)
	{
		if (a0exval > 8)
			return error(range_error);

		inst |= (a0exval & 7) << 9;
		D_word(inst);
	}
	else
	{
		fixup(FU_QUICK, sloc, a0expr);
		D_word(inst);
	}

	return 0;
}


//
// {bset, btst, bchg, bclr} -- #immed,ea -- Dn,ea
//
int m_bitop(WORD inst, WORD siz)
{
	// Enforce instruction sizes
	if (am1 == DREG)
	{                                        // X,Dn must be .n or .l 
		if (siz & (SIZB|SIZW))
			return error(siz_error);
	}
	else if (siz & (SIZW|SIZL))                                 // X,ea must be .n or .b 
		return error(siz_error);

	// Construct instr and EAs
	inst |= am1 | a1reg;

	if (am0 == IMMED)
	{
		D_word(inst);
		ea0gen(SIZB);                                         // Immediate bit number 
	}
	else
	{
		inst |= reg_9[a0reg];
		D_word(inst);
	}

	ea1gen(SIZB);                                            // ea to bit-munch 

	return 0;
}


int m_dbra(WORD inst, WORD siz)
{
	VALUE v;

	siz = siz;
	inst |= a0reg;
	D_word(inst);

	if (a1exattr & DEFINED)
	{
		if ((a1exattr & TDB) != cursect)
			return error(rel_error);

		v = a1exval - sloc;

		if (v + 0x8000 > 0x10000)
			return error(range_error);

		D_word(v);
	}
	else
	{
		fixup(FU_WORD|FU_PCREL|FU_ISBRA, sloc, a1expr);
		D_word(0);
	}

	return 0;
}


//
// EXG
//
int m_exg(WORD inst, WORD siz)
{
	int m;

	siz = siz;

	if (am0 == DREG && am1 == DREG)
		m = 0x0040;                                           // Dn,Dn 
	else if (am0 == AREG && am1 == AREG)
		m = 0x0048;                                           // An,An 
	else
	{
		if (am0 == AREG)
		{                                     // Dn,An or An,Dn 
			m = a1reg;                                         // Get AREG into a1reg 
			a1reg = a0reg;
			a0reg = m;
		}

		m = 0x0088;
	}

	inst |= m | reg_9[a0reg] | a1reg;
	D_word(inst);

	return 0;
}


//
// LINK
//
int m_link(WORD inst, WORD siz)
{
	siz = siz;
	inst |= a0reg;
	D_word(inst);
	ea1gen(SIZW);

	return 0;
}


//
// Handle MOVE <C_ALL> <C_ALTDATA>
//        MOVE <C_ALL> <M_AREG>
// 
// Optimize MOVE.L #<smalldata>,D0 to a MOVEQ
//
int m_move(WORD inst, int siz)
{
	// Try to optimize to MOVEQ
	if (siz == SIZL && am0 == IMMED && am1 == DREG
		&& (a0exattr & (TDB|DEFINED)) == DEFINED && a0exval + 0x80 < 0x100)
	{
		m_moveq((WORD)0x7000, (WORD)0);
	}
	else
	{
		inst |= siz_12[siz] | am_6[am1] | reg_9[a1reg] | am0 | a0reg;

		D_word(inst);

		if (am0 >= ADISP)
			ea0gen((WORD)siz);

		if (am1 >= ADISP)
			ea1gen((WORD)siz);
	}

	return 0;
}


//
// move USP,An -- move An,USP
//
int m_usp(WORD inst, WORD siz)
{
	siz = siz;

	if (am0 == AM_USP)
		inst |= a1reg;                         // USP,An 
	else
		inst |= a0reg;                                      // An,USP 

	D_word(inst);

	return 0;
}


//
// moveq
//
int m_moveq(WORD inst, WORD siz)
{
	siz = siz;

	if (!(a0exattr & DEFINED))
	{                              // Arrange for future fixup 
		fixup(FU_BYTE|FU_SEXT, sloc+1, a0expr);
		a0exval = 0; 
	}
	else if (a0exval + 0x100 >= 0x200)
		return error(range_error);

	inst |= reg_9[a1reg] | (a0exval & 0xff);
	D_word(inst);

	return 0;
}


//
// movep Dn,disp(An) -- movep disp(An),Dn
//
int m_movep(WORD inst, WORD siz)
{
	//WORD k;

	if (siz == SIZL)
		inst |= 0x0040;

	if (am0 == DREG)
	{
		inst |= reg_9[a0reg] | a1reg;
		D_word(inst);

		if (am1 == AIND)
		{
			D_word(0);
		}
		else 
			ea1gen(siz);
	}
	else
	{
		inst |= reg_9[a1reg] | a0reg;
		D_word(inst);

		if (am0 == AIND)
		{
			D_word(0);
		}
		else 
			ea0gen(siz);
	}

	return 0;
}


//
// Bcc -- BSR
//
int m_br(WORD inst, WORD siz)
{
	VALUE v;

	if (a0exattr & DEFINED)
	{
		if ((a0exattr & TDB) != cursect)
			return error(rel_error);

		v = a0exval - (sloc + 2);

		// Optimize branch instr. size
		if (siz == SIZN)
		{
			if (v != 0 && v + 0x80 < 0x100)
			{                   // Fits in .B 
				inst |= v & 0xff;
				D_word(inst);
				return 0;
			}
			else
			{                                           // Fits in .W 
				if (v + 0x8000 > 0x10000)
					return error(range_error);

				D_word(inst);
				D_word(v);
				return 0;
			}
		}

		if (siz == SIZB)
		{
			if (v + 0x80 >= 0x100)
				return error(range_error);

			inst |= v & 0xff;
			D_word(inst);
		}
		else
		{
			if (v + 0x8000 >= 0x10000)
				return error(range_error);

			D_word(inst);
			D_word(v);
		}

		return 0;
	}
	else if (siz == SIZN)
		siz = SIZW;

	if (siz == SIZB)
	{                                        // .B 
		fixup(FU_BBRA|FU_PCREL|FU_SEXT, sloc, a0expr);
		D_word(inst);
		return 0;
	}
	else
	{                                                 // .W 
		D_word(inst);
		fixup(FU_WORD|FU_PCREL|FU_LBRA|FU_ISBRA, sloc, a0expr);
		D_word(0);
	}

	return 0;
}


//
// ADDQ -- SUBQ
//
int m_addq(WORD inst, WORD siz)
{
	inst |= siz_6[siz] | am1 | a1reg;

	if (a0exattr & DEFINED)
	{
		if (a0exval > 8 ||	a0exval == 0)                       // Range in 1..8 
			return error(range_error);

		inst |= (a0exval & 7) << 9;
		D_word(inst);
	}
	else
	{
		fixup(FU_QUICK, sloc, a0expr);
		D_word(inst);
	}

	ea1gen(siz);

	return 0;
}


//
// trap #n
//
int m_trap(WORD inst, WORD siz)
{
	siz = siz;

	if (a0exattr & DEFINED)
	{
		if (a0exattr & TDB)
			return error(abs_error);

		if (a0exval >= 16)
			return error(range_error);

		inst |= a0exval;
		D_word(inst);
	}
	else 
		return error(undef_error);

	return 0;
}


//
// movem <rlist>,ea -- movem ea,<rlist>
//
int m_movem(WORD inst, WORD siz)
{
	VALUE eval;
	WORD i;
	WORD w;
	WORD rmask;

	if (siz & SIZB)
		return error("bad size suffix");

	if (siz == SIZL)
		inst |= 0x0040;

	if (*tok == '#')
	{                                        // Handle #<expr>,ea 
		++tok;

		if (abs_expr(&eval) != OK)
			return 0;

		if (eval >= 0x10000L)
			return error(range_error);

		rmask = (WORD)eval;
		goto immed1;
	}

	if (*tok >= KW_D0 && *tok <= KW_A7)
	{                     // <rlist>,ea 
		if (reglist(&rmask) < 0)
			return 0;

immed1:
		if (*tok++ != ',')
			return error("missing comma");

		if (amode(0) < 0)
			return 0;

		inst |= am0 | a0reg;

		if (!(amsktab[am0] & (C_ALTCTRL|M_APREDEC)))
			return error("invalid addressing mode");

		// If APREDEC, reverse register mask
		if (am0 == APREDEC)
		{
			w = rmask;
			rmask = 0;

			for(i=0x8000; i; i>>=1, w>>=1)
				rmask = (WORD)((rmask << 1) | w & 1);
		}
	}
	else
	{                                                 // ea,<rlist> 
		if (amode(0) < 0)
			return 0;

		inst |= 0x0400 | am0 | a0reg;

		if (*tok++ != ',')
			return error("missing comma");

		if (*tok == EOL)
			return error("missing register list");

		if (*tok == '#')
		{                                     // ea,#<expr> 
			++tok;

			if (abs_expr(&eval) != OK)
				return 0;

			if (eval >= 0x10000)
				return error(range_error);

			rmask = (WORD)eval;
		}
		else if (reglist(&rmask) < 0)
			return 0;

		if (!(amsktab[am0] & (C_CTRL|M_APOSTINC)))
			return error("invalid addressing mode");
	}

	D_word(inst);
	D_word(rmask);
	ea0gen(siz);

	return 0;
}


//
// CLR.x An ==> SUBA.x An,An
//
int m_clra(WORD inst, WORD siz)
{
	inst |= a0reg | reg_9[a0reg] | lwsiz_8[siz];
	D_word(inst);

	return 0;
}
