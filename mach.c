//
// RMAC - Reboot's Macro Assembler for all Atari computers
// MACH.C - Code Generation
// Copyright (C) 199x Landon Dyer, 2011-2017 Reboot and Friends
// RMAC derived from MADMAC v1.07 Written by Landon Dyer, 1986
// Source utilised with the kind permission of Landon Dyer
//

#include "mach.h"
#include "amode.h"
#include "direct.h"
#include "eagen.h"
#include "error.h"
#include "procln.h"
#include "riscasm.h"
//#include "rmac.h"
#include "sect.h"
#include "token.h"

#define DEF_KW
#include "kwtab.h"

// Exported variables
int movep = 0; // Global flag to indicate we're generating a movep instruction

// Function prototypes
int m_unimp(WORD, WORD), m_badmode(WORD, WORD), m_bad6mode(WORD, WORD), m_bad6inst(WORD, WORD);
int m_self(WORD, WORD);
int m_abcd(WORD, WORD);
int m_reg(WORD, WORD);
int m_imm(WORD, WORD);
int m_imm8(WORD, WORD);
int m_shi(WORD, WORD);
int m_shr(WORD, WORD);
int m_bitop(WORD, WORD);
int m_exg(WORD, WORD);
int m_ea(WORD, WORD);
int m_lea(WORD, WORD);
int m_br(WORD, WORD);
int m_dbra(WORD, WORD);
int m_link(WORD, WORD);
int m_adda(WORD, WORD);
int m_addq(WORD, WORD);
//int m_move(WORD, int);
int m_move(WORD, WORD);
int m_moveq(WORD, WORD);
int m_usp(WORD, WORD);
int m_movep(WORD, WORD);
int m_trap(WORD, WORD);
int m_movem(WORD, WORD);
int m_clra(WORD, WORD);

int m_move30(WORD, WORD);			//68020/30/40/60
int m_br30(WORD inst, WORD siz);
int m_ea030(WORD inst, WORD siz);
int m_bfop(WORD inst, WORD siz);
int m_callm(WORD inst, WORD siz);
int m_cas(WORD inst, WORD siz);
int m_cas2(WORD inst, WORD siz);
int m_chk2(WORD inst, WORD siz);
int m_cmp2(WORD inst, WORD siz);
int m_bkpt(WORD inst, WORD siz);
int m_cpbr(WORD inst, WORD siz);
int m_cpdbr(WORD inst, WORD siz);
int m_divs(WORD inst, WORD siz);
int m_muls(WORD inst, WORD siz);
int m_divu(WORD inst, WORD siz);
int m_mulu(WORD inst, WORD siz);
int m_divsl(WORD inst, WORD siz);
int m_divul(WORD inst, WORD siz);
int m_move16a(WORD inst, WORD siz);
int m_move16b(WORD inst, WORD siz);
int m_pack(WORD inst, WORD siz);
int m_rtm(WORD inst, WORD siz);
int m_rtd(WORD inst, WORD siz);
int m_trapcc(WORD inst, WORD siz);
int m_cinv(WORD inst, WORD siz);
int m_cprest(WORD inst, WORD siz);
int m_movec(WORD inst, WORD siz);
int m_moves(WORD inst, WORD siz);

// PMMU
int m_pbcc(WORD inst, WORD siz);
int m_pflusha(WORD inst, WORD siz);
int m_pflush(WORD inst, WORD siz);
int m_pflushr(WORD inst, WORD siz);
int m_pload(WORD inst, WORD siz);
int m_pmove(WORD inst, WORD siz);
int m_pmovefd(WORD inst, WORD siz);
int m_ptest(WORD inst, WORD siz);
int m_ptrapbs(WORD inst, WORD siz);
int m_ptrapbc(WORD inst, WORD siz);
int m_ptrapls(WORD inst, WORD siz);
int m_ptraplc(WORD inst, WORD siz);
int m_ptrapss(WORD inst, WORD siz);
int m_ptrapsc(WORD inst, WORD siz);
int m_ptrapas(WORD inst, WORD siz);
int m_ptrapac(WORD inst, WORD siz);
int m_ptrapws(WORD inst, WORD siz);
int m_ptrapwc(WORD inst, WORD siz);
int m_ptrapis(WORD inst, WORD siz);
int m_ptrapic(WORD inst, WORD siz);
int m_ptrapgc(WORD inst, WORD siz);
int m_ptrapgs(WORD inst, WORD siz);
int m_ptrapcs(WORD inst, WORD siz);
int m_ptrapcc(WORD inst, WORD siz);
int m_ptrapbsn(WORD inst, WORD siz);
int m_ptrapbcn(WORD inst, WORD siz);
int m_ptraplsn(WORD inst, WORD siz);
int m_ptraplcn(WORD inst, WORD siz);
int m_ptrapssn(WORD inst, WORD siz);
int m_ptrapscn(WORD inst, WORD siz);
int m_ptrapasn(WORD inst, WORD siz);
int m_ptrapacn(WORD inst, WORD siz);
int m_ptrapwsn(WORD inst, WORD siz);
int m_ptrapwcn(WORD inst, WORD siz);
int m_ptrapisn(WORD inst, WORD siz);
int m_ptrapicn(WORD inst, WORD siz);
int m_ptrapgsn(WORD inst, WORD siz);
int m_ptrapgcn(WORD inst, WORD siz);
int m_ptrapcsn(WORD inst, WORD siz);
int m_ptrapccn(WORD inst, WORD siz);

//FPU
int m_fabs(WORD inst, WORD siz);
int m_facos(WORD inst, WORD siz);
int m_fadd(WORD inst, WORD siz);
int m_fasin(WORD inst, WORD siz);
int m_fatan(WORD inst, WORD siz);
int m_fatanh(WORD inst, WORD siz);
int m_fcmp(WORD inst, WORD siz);
int m_fcos(WORD inst, WORD siz);
int m_fcosh(WORD inst, WORD siz);
int m_fdabs(WORD inst, WORD siz);
int m_fdadd(WORD inst, WORD siz);
int m_fdbcc(WORD inst, WORD siz);
int m_fddiv(WORD inst, WORD siz);
int m_fdfsqrt(WORD inst, WORD siz);
int m_fdiv(WORD inst, WORD siz);
int m_fdmove(WORD inst, WORD siz);
int m_fdmul(WORD inst, WORD siz);
int m_fdneg(WORD inst, WORD siz);
int m_fdsub(WORD inst, WORD siz);
int m_fetox(WORD inst, WORD siz);
int m_fetoxm1(WORD inst, WORD siz);
int m_fgetexp(WORD inst, WORD siz);
int m_fgetman(WORD inst, WORD siz);
int m_fint(WORD inst, WORD siz);
int m_fintrz(WORD inst, WORD siz);
int m_flog10(WORD inst, WORD siz);
int m_flog2(WORD inst, WORD siz);
int m_flogn(WORD inst, WORD siz);
int m_flognp1(WORD inst, WORD siz);
int m_fmod(WORD inst, WORD siz);
int m_fmove(WORD inst, WORD siz);
int m_fmovescr(WORD inst, WORD siz);
int m_fmovecr(WORD inst, WORD siz);
int m_fmovem(WORD inst, WORD siz);
int m_fmul(WORD inst, WORD siz);
int m_fneg(WORD inst, WORD siz);
int m_fnop(WORD inst, WORD siz);
int m_frem(WORD inst, WORD siz);
int m_fsabs(WORD inst, WORD siz);
int m_fsadd(WORD inst, WORD siz);
int m_fseq(WORD inst, WORD siz);
int m_fsne(WORD inst, WORD siz);
int m_fsgt(WORD inst, WORD siz);
int m_fsngt(WORD inst, WORD siz);
int m_fsge(WORD inst, WORD siz);
int m_fsnge(WORD inst, WORD siz);
int m_fslt(WORD inst, WORD siz);
int m_fsnlt(WORD inst, WORD siz);
int m_fsle(WORD inst, WORD siz);
int m_fsnle(WORD inst, WORD siz);
int m_fsgl(WORD inst, WORD siz);
int m_fsngl(WORD inst, WORD siz);
int m_fsgle(WORD inst, WORD siz);
int m_fsngle(WORD inst, WORD siz);
int m_fsogt(WORD inst, WORD siz);
int m_fsule(WORD inst, WORD siz);
int m_fsoge(WORD inst, WORD siz);
int m_fsult(WORD inst, WORD siz);
int m_fsolt(WORD inst, WORD siz);
int m_fsuge(WORD inst, WORD siz);
int m_fsole(WORD inst, WORD siz);
int m_fsugt(WORD inst, WORD siz);
int m_fsogl(WORD inst, WORD siz);
int m_fsueq(WORD inst, WORD siz);
int m_fsor(WORD inst, WORD siz);
int m_fsun(WORD inst, WORD siz);
int m_fsf(WORD inst, WORD siz);
int m_fst(WORD inst, WORD siz);
int m_fssf(WORD inst, WORD siz);
int m_fsst(WORD inst, WORD siz);
int m_fsseq(WORD inst, WORD siz);
int m_fssne(WORD inst, WORD siz);
int m_fscale(WORD inst, WORD siz);
int m_fsdiv(WORD inst, WORD siz);
int m_fsfsqrt(WORD inst, WORD siz);
int m_fsfsub(WORD inst, WORD siz);
int m_fsgldiv(WORD inst, WORD siz);
int m_fsglmul(WORD inst, WORD siz);
int m_fsin(WORD inst, WORD siz);
int m_fsincos(WORD inst, WORD siz);
int m_fsinh(WORD inst, WORD siz);
int m_fsmove(WORD inst, WORD siz);
int m_fsmul(WORD inst, WORD siz);
int m_fsneg(WORD inst, WORD siz);
int m_fsqrt(WORD inst, WORD siz);
int m_fsub(WORD inst, WORD siz);
int m_ftan(WORD inst, WORD siz);
int m_ftanh(WORD inst, WORD siz);
int m_ftentox(WORD inst, WORD siz);
int m_ftst(WORD inst, WORD siz);
int m_ftwotox(WORD inst, WORD siz);
int m_ftrapeq(WORD inst, WORD siz);
int m_ftrapne(WORD inst, WORD siz);
int m_ftrapgt(WORD inst, WORD siz);
int m_ftrapngt(WORD inst, WORD siz);
int m_ftrapge(WORD inst, WORD siz);
int m_ftrapnge(WORD inst, WORD siz);
int m_ftraplt(WORD inst, WORD siz);
int m_ftrapnlt(WORD inst, WORD siz);
int m_ftraple(WORD inst, WORD siz);
int m_ftrapnle(WORD inst, WORD siz);
int m_ftrapgl(WORD inst, WORD siz);
int m_ftrapngl(WORD inst, WORD siz);
int m_ftrapgle(WORD inst, WORD siz);
int m_ftrapngle(WORD inst, WORD siz);
int m_ftrapogt(WORD inst, WORD siz);
int m_ftrapule(WORD inst, WORD siz);
int m_ftrapoge(WORD inst, WORD siz);
int m_ftrapult(WORD inst, WORD siz);
int m_ftrapolt(WORD inst, WORD siz);
int m_ftrapuge(WORD inst, WORD siz);
int m_ftrapole(WORD inst, WORD siz);
int m_ftrapugt(WORD inst, WORD siz);
int m_ftrapogl(WORD inst, WORD siz);
int m_ftrapueq(WORD inst, WORD siz);
int m_ftrapor(WORD inst, WORD siz);
int m_ftrapun(WORD inst, WORD siz);
int m_ftrapf(WORD inst, WORD siz);
int m_ftrapt(WORD inst, WORD siz);
int m_ftrapsf(WORD inst, WORD siz);
int m_ftrapst(WORD inst, WORD siz);
int m_ftrapseq(WORD inst, WORD siz);
int m_ftrapsne(WORD inst, WORD siz);
int m_ftrapeqn(WORD inst, WORD siz);
int m_ftrapnen(WORD inst, WORD siz);
int m_ftrapgtn(WORD inst, WORD siz);
int m_ftrapngtn(WORD inst, WORD siz);
int m_ftrapgen(WORD inst, WORD siz);
int m_ftrapngen(WORD inst, WORD siz);
int m_ftrapltn(WORD inst, WORD siz);
int m_ftrapnltn(WORD inst, WORD siz);
int m_ftraplen(WORD inst, WORD siz);
int m_ftrapnlen(WORD inst, WORD siz);
int m_ftrapgln(WORD inst, WORD siz);
int m_ftrapngln(WORD inst, WORD siz);
int m_ftrapglen(WORD inst, WORD siz);
int m_ftrapnglen(WORD inst, WORD siz);
int m_ftrapogtn(WORD inst, WORD siz);
int m_ftrapulen(WORD inst, WORD siz);
int m_ftrapogen(WORD inst, WORD siz);
int m_ftrapultn(WORD inst, WORD siz);
int m_ftrapoltn(WORD inst, WORD siz);
int m_ftrapugen(WORD inst, WORD siz);
int m_ftrapolen(WORD inst, WORD siz);
int m_ftrapugtn(WORD inst, WORD siz);
int m_ftrapogln(WORD inst, WORD siz);
int m_ftrapueqn(WORD inst, WORD siz);
int m_ftraporn(WORD inst, WORD siz);
int m_ftrapunn(WORD inst, WORD siz);
int m_ftrapfn(WORD inst, WORD siz);
int m_ftraptn(WORD inst, WORD siz);
int m_ftrapsfn(WORD inst, WORD siz);
int m_ftrapstn(WORD inst, WORD siz);
int m_ftrapseqn(WORD inst, WORD siz);
int m_ftrapsnen(WORD inst, WORD siz);

// Common error messages
char range_error[] = "expression out of range";
char abs_error[] = "illegal absolute expression";
char seg_error[] = "bad (section) expression";
char rel_error[] = "illegal relative address";
char siz_error[] = "bad size specified";
char undef_error[] = "undefined expression";
char fwd_error[] = "forward or undefined expression";
char unsupport[] = "unsupported for selected CPU";

// Include code tables
MNTAB machtab[] = {
   { 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000, 0, m_badmode }, // 0
   #include "68ktab.h"
   {  0,  0L,  0L, 0x0000, 0, m_unimp   }            // Last entry
};

// Register number << 9
WORD reg_9[8] = {
	0, 1 << 9, 2 << 9, 3 << 9, 4 << 9, 5 << 9, 6 << 9, 7 << 9
};

// SIZB==>00, SIZW==>01, SIZL==>10, SIZN==>01 << 6
WORD siz_6[] = {
	(WORD)-1,                                        // n/a
	0,                                               // SIZB
	1<<6, (WORD)-1,                                  // SIZW, n/a
	2<<6, (WORD)-1, (WORD)-1, (WORD)-1,              // SIZL, n/a, n/a, n/a
	1<<6                                             // SIZN
};

// Byte/word/long size for MOVE instrs
WORD siz_12[] = {
   (WORD)-1,
   0x1000,                                           // Byte
   0x3000, (WORD)-1,                                 // Word
   0x2000, (WORD)-1, (WORD)-1, (WORD)-1,             // Long
   0x3000                                            // Word (SIZN)
};

// Word/long size (0=.w, 1=.l) in bit 8
WORD lwsiz_8[] = {
   (WORD)-1,                                         // n/a
   (WORD)-1,                                         // SIZB
   0, (WORD)-1,                                      // SIZW, n/a
   1<<8, (WORD)-1, (WORD)-1, (WORD)-1,               // SIZL, n/a, n/a, n/a
   0                                                 // SIZN
};

// Byte/Word/long size (0=.w, 1=.l) in bit 9
WORD lwsiz_9[] = {
	(WORD)-1,
	0,                                                      // Byte
	1<<9, (WORD)-1,                                         // Word
	1<<10, (WORD)-1, (WORD)-1, (WORD)-1,                    // Long
	1<<9                                                    // Word (SIZN)
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

// Control registers lookup table
WORD CREGlut[21] = {
	// MC68010/MC68020/MC68030/MC68040/CPU32
	0x000,		// Source Function Code(SFC)
	0x001,		// Destination Function Code(DFC)
	0x800,		// User Stack Pointer(USP)
	0x801,		// Vector Base Register(VBR)
	// MC68020 / MC68030 / MC68040
	0x002,		// Cache Control Register(CACR)
	0x802,		// Cache Address Register(CAAR) (020/030 only)
	0x803,		// Master Stack Pointer(MSP)
	0x804,		// Interrupt Stack Pointer(ISP)
	// MC68040 / MC68LC040
	0x003,		// MMU Translation Control Register(TC)
	0x004,		// Instruction Transparent Translation Register 0 (ITT0)
	0x005,		// Instruction Transparent Translation Register 1 (ITT1)
	0x006,		// Data Transparent Translation Register 0 (DTT0)
	0x007,		// Data Transparent Translation Register 1 (DTT1)
	0x805,		// MMU Status Register(MMUSR)
	0x806,		// User Root Pointer(URP)
	0x807,		// Supervisor Root Pointer(SRP)
	// MC68EC040 only
	0x004,		// Instruction Access Control Register 0 (IACR0)
	0x005,		// Instruction Access Control Register 1 (IACR1)
	0x006,		// Data Access Control Register 0 (DACR1)
	0x007,		// Data Access Control Register 1 (DACR1)
	// 68851 only
	0xFFF		// CPU Root Pointer (CRP) - There's no movec with CRP in it, this is just a guard entry
};


// Error messages
int m_unimp(WORD unused1, WORD unused2)
{
	return (int)error("unimplemented mnemonic");
}


//int m_badmode(void)
int m_badmode(WORD unused1, WORD unused2)
{
	return (int)error("inappropriate addressing mode");
}


int m_self(WORD inst, WORD usused)
{
	D_word(inst);
	return OK;
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
	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	return OK;
}


//
// Check if lea x(an),an can be optimised to addq.w #x,an--otherwise fall back
// to m_ea.
//
int m_lea(WORD inst, WORD siz)
{
	if (optim_flags[OPT_LEA_ADDQ]
		&& ((am0 == ADISP) && (a0reg == a1reg) && (a0exattr & DEFINED))
		&& ((a0exval > 0) && (a0exval <= 8)))
	{
		inst = B16(01010000, 01001000) | ((a0exval & 7) << 9) | (a0reg);
		D_word(inst);
		warn("lea size(An),An converted to addq #size,An");
		return OK;
	}

	return m_ea(inst, siz);
}


int m_ea030(WORD inst, WORD siz)
{
	CHECK00;
	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
		{
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		}
		else
		{
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
		}
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		if (am0 == AREG)
			// We get here if we're doing 020+ addressing and an address
			// register is used. For example, something like "tst a0". A bit of
			// a corner case, so kludge it
			a0reg = a0reg + 8;
		else if (am0 == PCDISP)
			//Another corner case (possibly!), so kludge ahoy
			inst |= am0;				// Get ea0 into instr
		else if (am0 == IMMED)
			inst |= am0 | a0reg;		// Get ea0 into instr
		else if (am0 == AM_CCR)
			inst |= am1 | a1reg;
		else if (am0 == AIND)
			inst |= am0;

		inst |= a0reg;					// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	return OK;
}


//
// Dx,Dy nnnnXXXnssnnnYYY If bit 0 of `inst' is set, install size bits in bits
// 6..7
//
int m_abcd(WORD inst, WORD siz)
{
	if (inst & 1)
	{
		// Install size bits
		inst--;
		inst |= siz_6[siz];
	}

	inst |= a0reg | reg_9[a1reg];
	D_word(inst);

	return OK;
}


//
// {adda} ea,AREG
//
int m_adda(WORD inst, WORD siz)
{
	inst |= am0 | a0reg | lwsiz_8[siz] | reg_9[a1reg];
	D_word(inst);
	ea0gen(siz);	// Generate EA

	return OK;
}


//
// If bit 0 of `inst' is 1, install size bits in bits 6..7 of instr.
// If bit 1 of `inst' is 1, install a1reg in bits 9..11 of instr.
//
int m_reg(WORD inst, WORD siz)
{
	if (inst & 1)
		// Install size bits
		inst |= siz_6[siz];

	if (inst & 2)
		// Install other register (9..11)
		inst |= reg_9[a1reg];

	inst &= ~7;			// Clear off crufty bits
	inst |= a0reg;		// Install first register
	D_word(inst);

	return OK;
}


//
// <op> #expr
//
int m_imm(WORD inst, WORD siz)
{
	D_word(inst);
	ea0gen(siz);

	return OK;
}


//
// <op>.b #expr
//
int m_imm8(WORD inst, WORD siz)
{
	siz = siz;
	D_word(inst);
	ea0gen(SIZB);

	return OK;
}


//
// <shift> Dn,Dn
//
int m_shr(WORD inst, WORD siz)
{
	inst |= reg_9[a0reg] | a1reg | siz_6[siz];
	D_word(inst);

	return OK;
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
		AddFixup(FU_QUICK, sloc, a0expr);
		D_word(inst);
	}

	return OK;
}


//
// {bset, btst, bchg, bclr} -- #immed,ea -- Dn,ea
//
int m_bitop(WORD inst, WORD siz)
{
	// Enforce instruction sizes
	if (am1 == DREG)
	{                               // X,Dn must be .n or .l
		if (siz & (SIZB | SIZW))
			return error(siz_error);
	}
	else if (siz & (SIZW | SIZL))	// X,ea must be .n or .b
		return error(siz_error);

	// Construct instr and EAs
	inst |= am1 | a1reg;

	if (am0 == IMMED)
	{
		D_word(inst);
		ea0gen(SIZB);				// Immediate bit number
	}
	else
	{
		inst |= reg_9[a0reg];
		D_word(inst);
	}

	// ea to bit-munch
	ea1gen(SIZB);

	return OK;
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
		AddFixup(FU_WORD | FU_PCREL | FU_ISBRA, sloc, a1expr);
		D_word(0);
	}

	return OK;
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

	return OK;
}


//
// LINK
//
int m_link(WORD inst, WORD siz)
{
	if (siz != SIZL)
	{
		// Is this an error condition???
	}
	else
	{
		CHECK00;
		inst &= ~((3 << 9) | (1 << 6) | (1 << 4));
		inst |= 1 << 3;
	}

	inst |= a0reg;
	D_word(inst);
	ea1gen(siz);

	return OK;
}


WORD extra_addressing[16]=
{
	0,     //0100 (bd,An,Xn)
	0,     //0101 ([bd,An],Xn,od)
	0x180, //0102 ([bc,An,Xn],od) (111 110 110 111)
	0,     //0103 (bd,PC,Xn)
	0,     //0104 ([bd,PC],Xn,od)
	0,     //0105 ([bc,PC,Xn],od)
	0,     //0106
	0,     //0107
	0,     //0110
	0,     //0111 Nothing
	0x30,  //0112 (Dn.w)
	0x30,  //0113 (Dn.l)
	0,     //0114
	0,     //0115
	0,     //0116
	0      //0117
};


//
// Handle MOVE <C_ALL> <C_ALTDATA>
//        MOVE <C_ALL> <M_AREG>
//
// Optimize MOVE.L #<smalldata>,D0 to a MOVEQ
//
int m_move(WORD inst, WORD size)
{
	// Cast the passed in value to an int
	int siz = (int)size;

	// Try to optimize to MOVEQ
	if (optim_flags[OPT_MOVEL_MOVEQ]
		&& (siz == SIZL) && (am0 == IMMED) && (am1 == DREG)
		&& ((a0exattr & (TDB | DEFINED)) == DEFINED)
		&& (a0exval + 0x80 < 0x100))
	{
		m_moveq((WORD)0x7000, (WORD)0);

		if (sbra_flag)
			warn("move.l #size,dx converted to moveq");
	}
	else
	{
		if ((am0 < ABASE) && (am1 < ABASE))			//68000 modes
		{
			inst |= siz_12[siz] | am_6[am1] | reg_9[a1reg] | am0 | a0reg;

			D_word(inst);

			if (am0 >= ADISP)
				ea0gen((WORD)siz);

			if (am1 >= ADISP)
				ea1gen((WORD)siz | 0x8000);   // Tell ea1gen we're move ea,ea
		}
		else					//68020+ modes
		{
			inst |= siz_12[siz] | reg_9[a1reg] | extra_addressing[am0 - ABASE];

			D_word(inst);

			if (am0 >= ADISP)
				ea0gen((WORD)siz);

			if (am1 >= ADISP)
				ea1gen((WORD)siz);
		}
	}

	return OK;
}

//
// Handle MOVE <C_ALL030> <C_ALTDATA>
//        MOVE <C_ALL030> <M_AREG>
//
int m_move30(WORD inst, WORD size)
{
	// Cast the passed in value to an int
	int siz = (int)size;
	inst |= siz_12[siz] | reg_9[a1reg & 7] | a0reg | extra_addressing[am0 - ABASE];

	D_word(inst);

	if (am0 >= ADISP)
		ea0gen((WORD)siz);

	if (am1 >= ADISP)
		ea1gen((WORD)siz);

	return OK;
}


//
// move USP,An -- move An,USP
//
int m_usp(WORD inst, WORD siz)
{
	siz = siz;

	if (am0 == AM_USP)
		inst |= a1reg;		// USP, An
	else
		inst |= a0reg;		// An, USP

	D_word(inst);

	return OK;
}


//
// moveq
//
int m_moveq(WORD inst, WORD siz)
{
	siz = siz;

	// Arrange for future fixup
	if (!(a0exattr & DEFINED))
	{
		AddFixup(FU_BYTE | FU_SEXT, sloc + 1, a0expr);
		a0exval = 0;
	}
	else if (a0exval + 0x100 >= 0x200)
		return error(range_error);

	inst |= reg_9[a1reg] | (a0exval & 0xFF);
	D_word(inst);

	return OK;
}


//
// movep Dn, disp(An) -- movep disp(An), Dn
//
int m_movep(WORD inst, WORD siz)
{
	// Tell ea0gen to lay off the 0(a0) optimisations on this one
    movep = 1;

	if (siz == SIZL)
		inst |= 0x0040;

	if (am0 == DREG)
	{
		inst |= reg_9[a0reg] | a1reg;
		D_word(inst);

		if (am1 == AIND)
			D_word(0)
		else
			ea1gen(siz);
	}
	else
	{
		inst |= reg_9[a1reg] | a0reg;
		D_word(inst);

		if (am0 == AIND)
			D_word(0)
		else
			ea0gen(siz);
	}

    movep = 0;
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
//{
//printf("m_br(): a0exattr = %X, cursect = %X, a0exval = %X, sloc = %X\n", a0exattr, cursect, a0exval, sloc);
			return error(rel_error);
//}

		v = a0exval - (sloc + 2);

		// Optimize branch instr. size
		if (siz == SIZN)
		{
			if (optim_flags[OPT_BSR_BCC_S] && (v != 0) && ((v + 0x80) < 0x100))
			{
				// Fits in .B
				inst |= v & 0xFF;
				D_word(inst);

				if (sbra_flag)
					warn("Bcc.w/BSR.w converted to .s");

				return OK;
			}
			else
			{
				// Fits in .W
				if ((v + 0x8000) > 0x10000)
					return error(range_error);

				D_word(inst);
				D_word(v);
				return OK;
			}
		}

		if (siz == SIZB || siz == SIZS)
		{
			if ((v + 0x80) >= 0x100)
				return error(range_error);

			inst |= v & 0xFF;
			D_word(inst);
		}
		else
		{
			if ((v + 0x8000) >= 0x10000)
				return error(range_error);

			D_word(inst);
			D_word(v);
		}

		return OK;
	}
	else if (siz == SIZN)
		siz = SIZW;

	if (siz == SIZB || siz == SIZS)
	{
		// .B
		AddFixup(FU_BBRA | FU_PCREL | FU_SEXT, sloc, a0expr);
		D_word(inst);
		return OK;
	}
	else
	{
		// .W
		D_word(inst);
		AddFixup(FU_WORD | FU_PCREL | FU_LBRA | FU_ISBRA, sloc, a0expr);
		D_word(0);
	}

	return OK;
}


//
// ADDQ -- SUBQ
//
int m_addq(WORD inst, WORD siz)
{
	inst |= siz_6[siz] | am1 | a1reg;

	if (a0exattr & DEFINED)
	{
		if ((a0exval > 8) || (a0exval == 0))	// Range in 1..8
			return error(range_error);

		inst |= (a0exval & 7) << 9;
		D_word(inst);
	}
	else
	{
		AddFixup(FU_QUICK, sloc, a0expr);
		D_word(inst);
	}

	ea1gen(siz);

	return OK;
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

	return OK;
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
	{
		// Handle #<expr>, ea
		tok++;

		if (abs_expr(&eval) != OK)
			return OK;

		if (eval >= 0x10000L)
			return error(range_error);

		rmask = (WORD)eval;
		goto immed1;
	}

	if ((*tok >= KW_D0) && (*tok <= KW_A7))
	{
		// <rlist>, ea
		if (reglist(&rmask) < 0)
			return OK;

immed1:
		if (*tok++ != ',')
			return error("missing comma");

		if (amode(0) < 0)
			return OK;

		inst |= am0 | a0reg;

		if (!(amsktab[am0] & (C_ALTCTRL | M_APREDEC)))
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
	{
		// ea, <rlist>
		if (amode(0) < 0)
			return OK;

		inst |= 0x0400 | am0 | a0reg;

		if (*tok++ != ',')
			return error("missing comma");

		if (*tok == EOL)
			return error("missing register list");

		if (*tok == '#')
		{
			// ea, #<expr>
			tok++;

			if (abs_expr(&eval) != OK)
				return OK;

			if (eval >= 0x10000)
				return error(range_error);

			rmask = (WORD)eval;
		}
		else if (reglist(&rmask) < 0)
			return OK;

		if (!(amsktab[am0] & (C_CTRL | M_APOSTINC)))
			return error("invalid addressing mode");
	}

	D_word(inst);
	D_word(rmask);
	ea0gen(siz);

	return OK;
}


//
// CLR.x An ==> SUBA.x An,An
//
int m_clra(WORD inst, WORD siz)
{
	inst |= a0reg | reg_9[a0reg] | lwsiz_8[siz];
	D_word(inst);

	return OK;
}


////////////////////////////////////////
//
// 68020/30/40 instructions
//
////////////////////////////////////////

//
// Bcc.l -- BSR.l
//
int m_br30(WORD inst, WORD siz)
{
	if (a0exattr & DEFINED)
	{
		if ((a0exattr & TDB) != cursect)
			return error(rel_error);

		VALUE v = a0exval - (sloc + 2);
		D_word(inst);
		D_long(v);

		return OK;
	}
	else
	{
		// .L
		AddFixup(FU_LONG | FU_PCREL | FU_SEXT, sloc, a0expr);
		D_word(inst);
		return OK;
	}
}


//
// bfchg, bfclr, bfexts, bfextu, bfffo, bfins, bfset
// (68020, 68030, 68040)
//
int m_bfop(WORD inst, WORD siz)
{
	//TODO: is this needed or can we put that in the mask in 68ktab???
	if (am0 == AREG || am0== APOSTINC || am0 == APREDEC || am0 == IMMED|| am0 == ABASE || am0 == MEMPOST || am0 == MEMPRE || am0 == PCBASE || am0 == PCMPOST || am0 == PCMPRE)
		return m_badmode(inst, siz);

	//First instruction word - just the opcode and first EA
	//Note: both am1 is ORed because solely of bfins - maybe it's a good idea to make a dedicated function for it?
	if (am1 == AM_NONE)
		am1 = 0;

	D_word((inst|am0|a0reg|am1|a1reg));
	ea0gen(siz);	// Generate EA

	//Second instruction word - Dest register (if exists), Do, Offset, Dw, Width
	inst = bfparam1 | bfparam2;

	if (am1 == DREG)
		inst |= a1reg << 12;

	if (am0 == DREG)
		inst |= a0reg << 12;

	D_word(inst);

	return OK;
}


//
// bkpt (68EC000, 68010, 68020, 68030, 68040, CPU32)
//
int m_bkpt(WORD inst, WORD siz)
{
	CHECK00;

	if (a0exattr & DEFINED)
	{
		if (a0exattr & TDB)
			return error(abs_error);

		if (a0exval >= 8)
			return error(range_error);

		inst |= a0exval;
		D_word(inst);
	}
	else
		return error(undef_error);

	return OK;
}


//
// callm (68020)
//
int m_callm(WORD inst, WORD siz)
{
	CHECKNO20;

	inst |= am1;
	D_word(inst);

	if (a0exattr & DEFINED)
	{
		if (a0exattr & TDB)
			return error(abs_error);

		if (a0exval > 255)
			return error(range_error);

		inst = a0exval;
		D_word(inst);
	}
	else
		return error(undef_error);

    ea1gen(siz);

	return OK;

}


//
// cas (68020, 68030, 68040)
//
int m_cas(WORD inst, WORD siz)
{
	WORD inst2;
	LONG amsk;
	int modes;

	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	switch (siz)
	{
	case SIZB:
		inst |= 1 << 9;
		break;
	case SIZW:
	case SIZN:
		inst |= 2 << 9;
		break;
	case SIZL:
		inst |= 3 << 9;
		break;
	default:
		return error("bad size suffix");
		break;
	}

	// Dc
	if ((*tok < KW_D0) && (*tok > KW_D7))
		return error("CAS accepts only data registers");

	inst2 = (*tok++) & 7;

	if (*tok++ != ',')
		return error("missing comma");

	// Du
	if ((*tok < KW_D0) && (*tok > KW_D7))
		return error("CAS accepts only data registers");

	inst2 |= ((*tok++) & 7) << 6;

	if (*tok++ != ',')
		return error("missing comma");

	// ea
	if ((modes = amode(1)) < 0)
		return OK;

	if (modes > 1)
		return error("too many ea fields");

	if (*tok!=EOL)
		return error("extra (unexpected) text found");

	// Reject invalud ea modes
	amsk = amsktab[am0];

	if (amsk & (M_AIND | M_APOSTINC | M_APREDEC | M_ADISP | M_AINDEXED | M_ABSW | M_ABSL | M_ABASE | M_MEMPOST | M_MEMPRE) == 0)
		return error("unsupported addressing mode");

	inst |= am0 | a0reg;
	D_word(inst);
	D_word(inst2);
	ea0gen(siz);

	return OK;
}


//
// cas2 (68020, 68030, 68040)
//
int m_cas2(WORD inst, WORD siz)
{
	WORD inst2, inst3;
	LONG amsk;

	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	switch (siz)
	{
	case SIZB:
		inst |= 1 << 9;
		break;
	case SIZW:
	case SIZN:
		inst |= 2 << 9;
		break;
	case SIZL:
		inst |= 3 << 9;
		break;
	default:
		return error("bad size suffix");
		break;
	}

	// Dc1
	if ((*tok < KW_D0) && (*tok > KW_D7))
		return error("CAS2 accepts only data registers for Dx1:Dx2 pairs");

	inst2 = (*tok++) & 7;

	if (*tok++ != ':')
		return error("missing colon");

	// Dc2
	if ((*tok < KW_D0) && (*tok > KW_D7))
		return error("CAS2 accepts only data registers for Dx1:Dx2 pairs");

	inst3 = (*tok++) & 7;

	if (*tok++ != ',')
		return error("missing comma");

	// Du1
	if ((*tok < KW_D0) && (*tok > KW_D7))
		return error("CAS2 accepts only data registers for Dx1:Dx2 pairs");

	inst2 |= ((*tok++) & 7) << 6;

	if (*tok++ != ':')
		return error("missing colon");

	// Du2
	if ((*tok < KW_D0) && (*tok > KW_D7))
		return error("CAS2 accepts only data registers for Dx1:Dx2 pairs");

	inst3 |= ((*tok++) & 7) << 6;

	if (*tok++ != ',')
		return error("missing comma");

	// Rn1
	if (*tok++ != '(')
		return error("missing (");
	if ((*tok >= KW_D0) && (*tok <= KW_D7))
		inst2 |= (((*tok++) & 7) << 12) | (0 << 15);
	else if ((*tok >= KW_A0) && (*tok <= KW_A7))
		inst2 |= (((*tok++) & 7) << 12) | (1 << 15);
	else
		return error("CAS accepts either data or address registers for Rn1:Rn2 pair");

	if (*tok++ != ')')
		return error("missing (");

	if (*tok++ != ':')
		return error("missing colon");

	// Rn2
	if (*tok++ != '(')
		return error("missing (");
	if ((*tok >= KW_D0) && (*tok <= KW_D7))
		inst3 |= (((*tok++) & 7) << 12) | (0 << 15);
	else if ((*tok >= KW_A0) && (*tok <= KW_A7))
		inst3 |= (((*tok++) & 7) << 12) | (1 << 15);
	else
		return error("CAS accepts either data or address registers for Rn1:Rn2 pair");

	if (*tok++ != ')')
		return error("missing (");

	if (*tok != EOL)
		return error("extra (unexpected) text found");

	D_word(inst);
	D_word(inst2);
	D_word(inst3);

	return OK;
}


//
// cmp2 (68020, 68030, 68040, CPU32)
//
int m_cmp2(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	switch (siz & 0x000F)
	{
	case SIZW:
	case SIZN:
		inst |= 1 << 9;
		break;
	case SIZL:
		inst |= 2 << 9;
		break;
	default:
		// SIZB
		break;
	}

	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	// If we're called from chk2 then bit 11 of size will be set. This is just
	// a dumb mechanism to pass this, required by the extension word. (You might
	// have noticed the siz & 15 thing above!)
	inst = (a1reg << 12) | (siz & (1 << 11));

	if (am1 == AREG)
		inst |= 1 << 15;

	D_word(inst);

	return OK;
}


//
// chk2 (68020, 68030, 68040, CPU32)
//
int m_chk2(WORD inst, WORD siz)
{
	return m_cmp2(inst, siz | (1 << 11));
}


//
// cpbcc(68020, 68030)
//
int m_cpbr(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030)) == 0)
		return error(unsupport);

	if (a0exattr & DEFINED)
	{
		if ((a0exattr & TDB) != cursect)
			return error(rel_error);

		VALUE v = a0exval - (sloc + 2);

		// Optimize branch instr. size
		if (siz == SIZL)
		{
			if ((v != 0) && ((v + 0x8000) < 0x10000))
			{
				inst |= (1 << 6);
				D_word(inst);
				WARNING(check what s "optional coprocessor-defined extension words!")
				D_long(v);
				return OK;
			}
		}
        else // SIZW/SIZN
		{
			if ((v + 0x8000) >= 0x10000)
				return error(range_error);

			D_word(inst);
			WARNING(check what s "optional coprocessor-defined extension words!")
			D_word(v);
		}

		return OK;
	}
	else if (siz == SIZN)
		siz = SIZW;

	if (siz == SIZL)
	{
		// .L
        D_word(inst);
        AddFixup(FU_LONG | FU_PCREL | FU_SEXT, sloc, a0expr);
        D_long(0);
		return OK;
	}
	else
	{
		// .W
		D_word(inst);
		AddFixup(FU_WORD | FU_PCREL | FU_SEXT, sloc, a0expr);
		D_word(0);
	}

	return OK;
}


//
// cpdbcc(68020, 68030)
//
int m_cpdbr(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030)) == 0)
		return error(unsupport);

	return error("Not implemented yet.");

	//return OK;
}


//
// divs.l
//
int m_divs(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	inst = a1reg + (a2reg << 12) + (1 << 11);
	D_word(inst);

	return OK;
}


//
// muls.l
//
int m_muls(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	inst = a1reg + (a2reg << 12) + (1 << 11);
	inst |= mulmode;  // add size bit
	D_word(inst);

	return OK;
}


//
// divu.l
//
int m_divu(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	//WARNING("divu.l d0,d1 is actually divul.l d0,d1:d1!!!")

	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

										// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	inst = a1reg + (a2reg << 12);
	D_word(inst);

	return OK;
}


//
// mulu.l
//
int m_mulu(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	inst = a1reg + (a2reg << 12);
    inst |= mulmode;  // add size bit
	D_word(inst);

	return OK;
}


//
// divsl.l
//
int m_divsl(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	inst = a1reg + (a2reg << 12) + (1 << 11) + (1 << 10);
	D_word(inst);

	return OK;
}

//
// divul.l
//
int m_divul(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	inst = a1reg + (a2reg << 12) + (1 << 10);
	D_word(inst);

	return OK;
}


//
// move16 (ax)+,(ay)+
//
int m_move16a(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68040 | CPU_68060)) == 0)
		return error(unsupport);

	inst |= a0reg;
	D_word(inst);
	inst = (1 << 15) + (a1reg << 12);
	D_word(inst);

	return OK;
}


//
// move16 with absolute address
//
int m_move16b(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68040 | CPU_68060)) == 0)
		return error(unsupport);

	int v;
	inst |= a1reg;
	D_word(inst);

	if (am0 == APOSTINC)
	{
		if (am1 == AIND)
			return error("Wasn't this suppose to call m_move16a???");
		else
		{
			//move16 (ax)+,(xxx).L
			inst |= 0 << 3;
			v = a1exval;
		}
	}
	else if (am0 == ABSL)
	{
		if (am1 == AIND)
		{
			//move16 (xxx).L,(ax)+
			inst |= 1 << 3;
			v = a0exval;
		}
		else //APOSTINC
		{
			//move16 (xxx).L,(ax)
			inst |= 3 << 3;
			v = a0exval;
		}
	}
	else if (am0 == AIND)
	{
		//move16 (ax),(xxx).L
		inst |= 2 << 3;
		v = a1exval;
	}

	D_word(inst);
	D_long(v);

	return OK;
}


//
// pack/unpack
//
int m_pack(WORD inst, WORD siz)
{
	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	WARNING(Parsing stuff by hand here might be better)

#if 0
	if ((am0 == DREG) && (am1 == DREG))
	{
		inst |= (1 << 3) + (a0reg << 9) + (a1reg);
	}
	else if ((am0 == APREDEC) && (am1 == APREDEC))
	{
		inst |= (a0reg << 9) + (a1reg);
	}
	else
		return error("Only allowed combinations for pack/unpack are -(ax),-(ay) and dx,dy.");

	D_word(inst);
#endif

	return OK;
}


//
// rtm Rn
//
int m_rtm(WORD inst, WORD siz)
{
	CHECKNO20;

	if (am0 == DREG)
	{
		inst |= a0reg;
	}
	else if (am0 == AREG)
	{
		inst |= (1 << 3) + a0reg;
	}
	else
		return error("rtm only allows data or address registers.");

	D_word(inst);

	return OK;
}


//
// rtd #n
//
int m_rtd(WORD inst, WORD siz)
{
	CHECK00;

	if (a0exattr & DEFINED)
	{
		if (a0exattr & TDB)
			return error(abs_error);

		if ((a0exval + 0x8000) <= 0x7FFF)
			return error(range_error);

		D_word(inst);
		D_word(a0exval);
	}
	else
		return error(undef_error);

	return OK;
}


//
// trapcc
//
int m_trapcc(WORD inst, WORD siz)
{
	CHECK00;

	if (am0 == AM_NONE)
	{
		D_word(inst);
	}
	else if (am0 == IMMED)
	{
		if (siz == SIZW)
		{
			if (a0exval < 0x10000)
			{
				inst |= 2;
				D_word(inst);
				D_word(a0exval);
			}
			else
				return error("Immediate value too big");
		}
		else //DOTL
		{
			inst |= 3;
			D_word(inst);
			D_long(a0exval);
		}
	}
	else
		return error("Invalid parameter for trapcc");

	return OK;
}


//
// cinv (68040)
//
int m_cinv(WORD inst, WORD siz)
{
	CHECKNO40;
	WARNING("cinvl ,(an) / cinvp ,(an) / cinva should work!")

	if (am0 == AM_NONE)
		inst |= (0 << 6) | (a1reg);
	else if (am0 == KW_IC40)
		inst |= (2 << 6) | (a1reg);
	else if (am0 == KW_DC40)
		inst |= (1 << 6) | (a1reg);
	else if (am0 == KW_BC40)
		inst |= (3 << 6) | (a1reg);

	D_word(inst);
	return OK;
}


//
// cpRESTORE (68020, 68030)
//
int m_cprest(WORD inst, WORD siz)
{
	if (activecpu & !(CPU_68020 | CPU_68030))
		return error(unsupport);

	inst |= am0 | a0reg;
	D_word(inst);
	ea0gen(siz);

	return OK;
}


//
// movec (68010, 68020, 68030, 68040, CPU32)
//
int m_movec(WORD inst, WORD siz)
{
	CHECK00;

	if (am0 == DREG || am0 == AREG)
	{
		// movec Rn,Rc
		inst |= 1;
		D_word(inst);

		if (am0 == DREG)
		{
			inst = (0 << 15) + (a0reg << 12) + CREGlut[a1reg];
			D_word(inst);
		}
		else
		{
			inst = (1 << 15) + (a0reg << 12) + CREGlut[a1reg];
			D_word(inst);
		}
	}
	else
	{
		// movec Rc,Rn
		D_word(inst);

		if (am1 == DREG)
		{
			inst = (0 << 15) + (a1reg << 12) + CREGlut[a0reg];
			D_word(inst);
		}
		else
		{
			inst = (1 << 15) + (a1reg << 12) + CREGlut[a0reg];
			D_word(inst);
		}
	}

	return OK;
}


//
// moves (68010, 68020, 68030, 68040, CPU32)
//
int m_moves(WORD inst, WORD siz)
{
	if (activecpu & !(CPU_68020 | CPU_68030 | CPU_68040))
		return error(unsupport);

	if (siz == SIZB)
	{
		inst |= 0 << 6;
	}
	else if (siz == SIZL)
	{
		inst |= 2 << 6;
	}
	else // SIZW/SIZN
	{
		inst |= 1 << 6;
	}

	if (am0 == DREG)
	{
		inst |= am1 | a1reg;
		D_word(inst);
		inst = (a0reg << 12) | (1 << 11) | (0 << 15);
		D_word(inst);
	}
	else if (am0 == AREG)
	{
		inst |= am1 | a1reg;
		D_word(inst);
		inst = (a0reg << 12) | (1 << 11) | (1 << 15);
		D_word(inst);
	}
	else
	{
		if (am1 == DREG)
		{
			inst |= am0 | a0reg;
			D_word(inst);
			inst = (a1reg << 12) | (0 << 11) | (0 << 15);
			D_word(inst);
		}
		else
		{
			inst |= am0 | a0reg;
			D_word(inst);
			inst = (a1reg << 12) | (0 << 11) | (1 << 15);
			D_word(inst);
		}
	}

	return OK;
}


//
// PBcc (MC68851)
//
int m_pbcc(WORD inst, WORD siz)
{
	CHECKNO20;
	return error("Not implemented yet.");
}


//
// pflusha (68030)
//
int m_pflusha(WORD inst, WORD siz)
{
	CHECKNO30;

	D_word(inst);
	inst = (1 << 13) | (1 << 10) | (0 << 5) | 0;
	D_word(inst);
	return OK;
}


//
// pflush (68030, 68040, 68060)
//
int m_pflush(WORD inst, WORD siz)
{
	if (activecpu == CPU_68030)
	{
		D_word(inst);
		D_word((1 << 13) | (1 << 10) | (0 << 5) | 0);
	}
	else if (activecpu == CPU_68040 || activecpu == CPU_68060)
	{
		D_word(0xf918);
	}
	else
		return error(unsupport);

	return OK;
}


//
// pflushr (68551)
//
int m_pflushr(WORD inst, WORD siz)
{
	CHECKNO20;

	WORD flg = inst;					// Save flag bits
	inst &= ~0x3F;						// Clobber flag bits in instr

	// Install "standard" instr size bits
	if (flg & 4)
		inst |= siz_6[siz];

	if (flg & 16)
	{
		// OR-in register number
		if (flg & 8)
			inst |= reg_9[a1reg];		// ea1reg in bits 9..11
		else
			inst |= reg_9[a0reg];		// ea0reg in bits 9..11
	}

	if (flg & 1)
	{
		// Use am1
		inst |= am1 | a1reg;			// Get ea1 into instr
		D_word(inst);					// Deposit instr

		// Generate ea0 if requested
		if (flg & 2)
			ea0gen(siz);

		ea1gen(siz);					// Generate ea1
	}
	else
	{
		// Use am0
		inst |= am0 | a0reg;			// Get ea0 into instr
		D_word(inst);					// Deposit instr
		ea0gen(siz);					// Generate ea0

		// Generate ea1 if requested
		if (flg & 2)
			ea1gen(siz);
	}

	D_word(B16(10100000, 00000000));
}


//
// ploadr, ploadw (68030)
//
int m_pload(WORD inst, WORD siz)
{
	CHECKNO30;
	return error("Not implemented yet.");
}


//
// pmove (68030)
//
int m_pmove(WORD inst, WORD siz)
{
	int inst2,reg;

	CHECKNO30;

	inst2 = inst & (1 << 8); //Copy the flush bit over to inst2 in case we're called from m_pmovefd
	inst &= ~(1 << 8);		//And mask it out

	if (am0 == CREG)
	{
		reg = a0reg;
		inst2 |= (1 << 9);
	}
	else if (am1 == CREG)
	{
		reg = a1reg;
		inst2 |= 0;
	}
	else
		return error("pmove sez: Wut?");

	if (((reg == (KW_URP - KW_SFC)) || (reg == (KW_SRP - KW_SFC)))
		&& ((siz != SIZD) && (siz != SIZN)))
		return error(siz_error);

	if (((reg == (KW_TC - KW_SFC)) || (reg == (KW_TT0 - KW_SFC)) || (reg == (KW_TT1 - KW_SFC)))
		&& ((siz != SIZL) && (siz != SIZN)))
		return error(siz_error);

	if ((reg == (KW_MMUSR - KW_SFC)) && ((siz != SIZW) && (siz != SIZN)))
		return error(siz_error);

	WARNING(Not all addressing modes are legal here!)

	if (am0 == CREG)
	{
		inst |= am1;
		D_word(inst);
		ea1gen(siz);
	}
	else if (am1 == CREG)
	{
		inst |= am0;
		D_word(inst);
		ea0gen(siz);
	}

	switch (reg)
	{
	case (KW_URP - KW_SFC):
		inst2 |= (3 << 10) + (2 << 13); break;
	case (KW_SRP - KW_SFC):
		inst2 |= (2 << 10) + (2 << 13); break;
	case (KW_TC - KW_SFC):
		inst2 |= (0 << 10) + (2 << 13); break;
	case (KW_TT0 - KW_SFC):
		inst2 |= (2 << 10) + (0 << 13); break;
	case (KW_TT1 - KW_SFC):
		inst2 |= (3 << 10) + (0 << 13); break;
	case (KW_MMUSR - KW_SFC):
		inst2 |= (3 << 10) + (3 << 13); break;
	case (KW_CRP - KW_SFC) :	//68851 only
		inst2 |= (3 << 10) + (2 << 13); break;
	}

	D_word(inst2);

	return OK;
}


//
// pmovefd (68030)
//
int m_pmovefd(WORD inst, WORD siz)
{
	CHECKNO30;

	return m_pmove(inst | (1 << 8), siz);
}


//
// ptrapcc (68851)
//
int m_ptrapbs(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00000000)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00000000)); D_long(a0exval); } return OK; }
int m_ptrapbc(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00000001)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00000001)); D_long(a0exval); } return OK; }
int m_ptrapls(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00000010)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00000010)); D_long(a0exval); } return OK; }
int m_ptraplc(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00000011)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00000011)); D_long(a0exval); } return OK; }
int m_ptrapss(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00000100)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00000100)); D_long(a0exval); } return OK; }
int m_ptrapsc(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00000101)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00000101)); D_long(a0exval); } return OK; }
int m_ptrapas(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00000110)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00000110)); D_long(a0exval); } return OK; }
int m_ptrapac(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00000111)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00000111)); D_long(a0exval); } return OK; }
int m_ptrapws(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00001000)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00001000)); D_long(a0exval); } return OK; }
int m_ptrapwc(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00001001)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00001001)); D_long(a0exval); } return OK; }
int m_ptrapis(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00001010)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00001010)); D_long(a0exval); } return OK; }
int m_ptrapic(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00001011)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00001011)); D_long(a0exval); } return OK; }
int m_ptrapgc(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00001100)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00001100)); D_long(a0exval); } return OK; }
int m_ptrapgs(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00001101)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00001101)); D_long(a0exval); } return OK; }
int m_ptrapcs(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00001110)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00001110)); D_long(a0exval); } return OK; }
int m_ptrapcc(WORD inst, WORD siz) { CHECKNO20; if (siz == SIZW) { D_word(inst); D_word(B8(00001111)); D_word(a0exval); } else { inst |= 3; D_word(inst); D_word(B8(00001111)); D_long(a0exval); } return OK; }
int m_ptrapbsn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00000000)); return OK; }
int m_ptrapbcn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00000001)); return OK; }
int m_ptraplsn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00000010)); return OK; }
int m_ptraplcn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00000011)); return OK; }
int m_ptrapssn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00000100)); return OK; }
int m_ptrapscn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00000101)); return OK; }
int m_ptrapasn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00000110)); return OK; }
int m_ptrapacn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00000111)); return OK; }
int m_ptrapwsn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00001000)); return OK; }
int m_ptrapwcn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00001001)); return OK; }
int m_ptrapisn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00001010)); return OK; }
int m_ptrapicn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00001011)); return OK; }
int m_ptrapgsn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00001100)); return OK; }
int m_ptrapgcn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00001101)); return OK; }
int m_ptrapcsn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00001110)); return OK; }
int m_ptrapccn(WORD inst, WORD siz) { CHECKNO20; D_word(inst); D_word(B8(00001111)); return OK; }


//
// ptestr, ptestw (68030)
//
int m_ptest(WORD inst, WORD siz)
{
	CHECKNO30;

	if (activecpu == CPU_68030)
		return error("Not implemented yet.");
	else if (activecpu == CPU_68040)
		return error("Not implemented yet.");

	return ERROR;
}


#define FPU_NOWARN 0
#define FPU_P_EMUL 1
#define FPU_P2_EMU 2
#define FPU_FPSP   4


//
// Generate a FPU opcode
//
static inline int gen_fpu(WORD inst, WORD siz, WORD opmode, WORD emul)
{
	if (am0 < AM_NONE)	// Check first operand for ea or fp - is this right?
	{
		inst |= (1 << 9);	// Bolt on FPU id
		inst |= am0;

		if (am0 == DREG)
            inst |= a0reg;

		D_word(inst);
		inst = 1 << 14; // R/M field (we have ea so have to set this to 1)

		switch (siz)
		{
		case SIZB: 	inst |= (6 << 10); break;
		case SIZW:	inst |= (4 << 10); break;
		case SIZL:	inst |= (0 << 10); break;
		case SIZN:
		case SIZS:	inst |= (1 << 10); break;
		case SIZD:	inst |= (5 << 10); break;
		case SIZX:	inst |= (2 << 10); break;
		case SIZP:
			inst |= (3 << 10);

			if (emul)
				warn("This encoding will cause an unimplemented data type exception in the MC68040 to allow emulation in software.");

			break;
		default:
			return error("Something bad happened, possibly, in gen_fpu.");
			break;
		}

		inst |= (a1reg << 7);
		inst |= opmode;
		D_word(inst);
		ea0gen(siz);
	}
	else
	{
		inst |= (1 << 9);	//Bolt on FPU id
		D_word(inst);
		inst = 0;
		inst = a0reg << 10;
		inst |= (a1reg << 7);
		inst |= opmode;
		D_word(inst);
	}

	if ((emul & FPU_FPSP) && (activefpu == FPU_68040))
		warn("Instruction is emulated in 68040");

	return OK;
}


//
// fabs, fsabs, fdabs (6888X, 68040)
//
int m_fabs(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00011000), FPU_P_EMUL);
}


int m_fsabs(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01011000), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


int m_fdabs(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01011100), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


//
// facos (6888X, 68040FPSP)
//
int m_facos(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00011100), FPU_FPSP);
}


//
// fadd (6888X, 68040FPSP)
//
int m_fadd(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00100010), FPU_P_EMUL);
}


int m_fsadd(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01100010), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


int m_fdadd(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01100110), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


//
// fasin (6888X, 68040FPSP)f
//
int m_fasin(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00001100), FPU_FPSP);
}


//
// fatan (6888X, 68040FPSP)
//
int m_fatan(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00001010), FPU_FPSP);
}


//
// fatanh (6888X, 68040FPSP)
//
int m_fatanh(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00001101), FPU_FPSP);
}


//
// fcmp (6888X, 68040)
//
int m_fcmp(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00111000), FPU_P_EMUL);
}


//
// fcos (6888X, 68040FPSP)
//
int m_fcos(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00011101), FPU_FPSP);
}


//
// fcosh (6888X, 68040FPSP)
//
int m_fcosh(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00011001), FPU_FPSP);
}


//
// fdbcc (6888X, 68040)
//
int m_fdbcc(WORD inst, WORD siz)
{
	WORD opcode = inst & 0x3F;	//Grab conditional bitfield

	inst &= ~0x3F;
	inst |= 1 << 3;

	siz = siz;
	inst |= a0reg;
	D_word(inst);
	D_word(opcode);

	if (a1exattr & DEFINED)
	{
		if ((a1exattr & TDB) != cursect)
			return error(rel_error);

		VALUE v = a1exval - sloc;

		if ((v + 0x8000) > 0x10000)
			return error(range_error);

		D_word(v);
	}
	else
	{
		AddFixup(FU_WORD | FU_PCREL | FU_ISBRA, sloc, a1expr);
		D_word(0);
	}

	return OK;
}


//
// fdiv (6888X, 68040)
//
int m_fdiv(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00100000), FPU_P_EMUL);
}


int m_fsdiv(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01100000), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


int m_fddiv(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01100100), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


//
// fetox (6888X, 68040FPSP)
//
int m_fetox(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00010000), FPU_FPSP);
}


//
// fetoxm1 (6888X, 68040FPSP)
//
int m_fetoxm1(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00001000), FPU_FPSP);
}


//
// fgetexp (6888X, 68040FPSP)
//
int m_fgetexp(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00011110), FPU_FPSP);
}


//
// fgetman (6888X, 68040FPSP)
//
int m_fgetman(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00011111), FPU_FPSP);
}


//
// fint (6888X, 68040FPSP)
//
int m_fint(WORD inst, WORD siz)
{
	if (am1 == AM_NONE)
		// special case - fint fpx = fint fpx,fpx
		a1reg = a0reg;

	return gen_fpu(inst, siz, B8(00000001), FPU_FPSP);
}


//
// fintrz (6888X, 68040FPSP)
//
int m_fintrz(WORD inst, WORD siz)
{
	if (am1 == AM_NONE)
		// special case - fintrz fpx = fintrz fpx,fpx
		a1reg = a0reg;

	return gen_fpu(inst, siz, B8(00000011), FPU_FPSP);
}


//
// flog10 (6888X, 68040FPSP)
//
int m_flog10(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00010101), FPU_FPSP);
}


//
// flog2 (6888X, 68040FPSP)
//
int m_flog2(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00010110), FPU_FPSP);
}


//
// flogn (6888X, 68040FPSP)
//
int m_flogn(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00010100), FPU_FPSP);
}


//
// flognp1 (6888X, 68040FPSP)
//
int m_flognp1(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00000110), FPU_FPSP);
}


//
// fmod (6888X, 68040FPSP)
//
int m_fmod(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00100001), FPU_FPSP);
}


//
// fmove (6888X, 68040)
//
int m_fmove(WORD inst, WORD siz)
{

	// EA to register
	if ((am0 == FREG) && (am1 < AM_USP))
	{
		// EA
		inst |= am1 | a1reg;
		D_word(inst);

		// R/M
		inst = 3 << 13;

		WARNING("K-factor logic is totally bogus - fix!")

		// Source specifier
		switch (siz)
		{
		case SIZB: 	inst |= (6 << 10); break;
		case SIZW:	inst |= (4 << 10); break;
		case SIZL:	inst |= (0 << 10); break;
		case SIZN:
		case SIZS:	inst |= (1 << 10); break;
		case SIZD:	inst |= (5 << 10); break;
		case SIZX:	inst |= (2 << 10); break;
		case SIZP:  inst |= (3 << 10);
			if (bfparam1)
				inst |= 1 << 12;

			inst |= (bfparam1 & 0x7FF) >> 2;
			break;
		default:
			return error("Something bad happened, possibly.");
			break;
		}

		// Immediate {} value
		if (bf0exval >= (1 << 6))
			return error("K-factor must be between 0 and 31");

		if (!bfparam1 && (siz == SIZP))
            inst |= bf0exval;

		// Destination specifier
		inst |= (a0reg << 7);

		// Opmode
		inst |= 0;

		D_word(inst);
		ea1gen(siz);
	}
	else if ((am0 < AM_USP) && (am1 == FREG))
	{
		// EA
		inst |= am0 | a0reg;
		D_word(inst);

		// R/M
		inst = 1 << 14;

		// Source specifier
		switch (siz)
		{
		case SIZB: 	inst |= (6 << 10); break;
		case SIZW:	inst |= (4 << 10); break;
		case SIZL:	inst |= (0 << 10); break;
		case SIZN:
		case SIZS:	inst |= (1 << 10); break;
		case SIZD:	inst |= (5 << 10); break;
		case SIZX:	inst |= (2 << 10); break;
		case SIZP:  inst |= (3 << 10); break;
		default:
			return error("Something bad happened, possibly.");
			break;
		}

		// Destination specifier
		inst |= (a1reg << 7);

		// Opmode
		inst |= 0;

		D_word(inst);
		ea0gen(siz);
	}
	else if ((am0 == FREG) && (am1 == FREG))
	{
		// EA
		D_word(inst);

		// R/M
		inst = 0 << 14;

		// Source specifier
		if (siz != SIZX)
			return error("Invalid size");

		// Source register
		inst |= (a0reg << 10);

        // Destination register
		inst |= (a1reg << 7);

		D_word(inst);
	}

	return OK;
}


//
// fmove (6888X, 68040)
//
int m_fmovescr(WORD inst, WORD siz)
{
	// Move Floating-Point System Control Register (FPCR)
	// ea
	// dr
	// Register select
	if ((am0 == FPSCR) && (am1 < AM_USP))
	{
		inst |= am1 | a1reg;
		D_word(inst);
		inst = (1 << 13) + (1 << 15);
		inst |= a0reg;
		D_word(inst);
		ea1gen(siz);
		return OK;
	}
	else if ((am1 == FPSCR) && (am0 < AM_USP))
	{
		inst |= am0 | a0reg;
		D_word(inst);
		inst = (0 << 13) + (1 << 15);
		inst |= a1reg;
		D_word(inst);
		ea0gen(siz);
		return OK;
	}
	else
		return error("m_fmovescr says: wut?");
}


//
// fsmove/fdmove (68040)
//
int m_fsmove(WORD inst, WORD siz)
{
	return error("Not implemented yet.");

#if 0
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01100100), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
#endif
}


int m_fdmove(WORD inst, WORD siz)
{
	return error("Not implemented yet.");

#if 0
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01100100), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
#endif
}


//
// fmovecr (6888X, 68040FPSP)
//
int m_fmovecr(WORD inst, WORD siz)
{
	D_word(inst);
	inst = 0x5c00;
	inst |= a1reg << 7;
	inst |= a0exval;
	D_word(inst);

	if (activefpu == FPU_68040)
		warn("Instruction is emulated in 68040");

	return OK;
}


//
// fmovem (6888X, 68040)
//
int m_fmovem(WORD inst, WORD siz)
{
	WORD regmask;
	WORD datareg;

	if (siz == SIZX)
	{
		if ((*tok >= KW_FP0) && (*tok <= KW_FP7))
		{
			// fmovem.x <rlist>,ea
			if (fpu_reglist_left(&regmask) < 0)
				return OK;

			if (*tok++ != ',')
				return error("missing comma");

			if (amode(0) < 0)
				return OK;

			inst |= am0 | a0reg;

			if (!(amsktab[am0] & (C_ALTCTRL | M_APREDEC)))
				return error("invalid addressing mode");

			D_word(inst);
			inst = (1 << 15) | (1 << 14) | (1 << 13) | (0 << 11) | regmask;
			D_word(inst);
			ea0gen(siz);
			return OK;
		}
		else if ((*tok >= KW_D0) && (*tok <= KW_D7))
		{
			// fmovem.x Dn,ea
			datareg = (*tok++ & 7) << 10;

			if (*tok++ != ',')
				return error("missing comma");

			if (amode(0) < 0)
				return OK;

			inst |= am0 | a0reg;

			if (!(amsktab[am0] & (C_ALTCTRL | M_APREDEC)))
				return error("invalid addressing mode");

			D_word(inst);
			inst = (1 << 15) | (1 << 14) | (1 << 13) | (1 << 11) | (datareg << 4);
			D_word(inst);
			ea0gen(siz);
			return OK;
		}
		else
		{
			// fmovem.x ea,...
			if (amode(0) < 0)
				return OK;

			inst |= am0 | a0reg;

			if (*tok++ != ',')
				return error("missing comma");

			if ((*tok >= KW_FP0) && (*tok <= KW_FP7))
			{
				//fmovem.x ea,<rlist>
				if (fpu_reglist_right(&regmask) < 0)
					return OK;

				D_word(inst);
				inst = (1 << 15) | (1 << 14) | (0 << 13) | (2 << 11) | regmask;
				D_word(inst);
				ea0gen(siz);
				return OK;
			}
			else
			{
				// fmovem.x ea,Dn
				datareg = (*tok++ & 7) << 10;
				D_word(inst);
				inst = (1 << 15) | (1 << 14) | (0 << 13) | (3 << 11) | (datareg << 4);
				D_word(inst);
				ea0gen(siz);
				return OK;
			}
		}
	}
	else if ((siz == SIZL) || (siz==SIZN))
	{
		if ((*tok == KW_FPCR) || (*tok == KW_FPSR) || (*tok == KW_FPIAR))
		{
			//fmovem.l <rlist>,ea
			regmask = (1 << 15) | (1 << 13);
fmovem_loop_1:
			if (*tok == KW_FPCR)
			{
				regmask |= (1 << 12);
				tok++;
				goto fmovem_loop_1;
			}

			if (*tok == KW_FPSR)
			{
				regmask |= (1 << 11);
				tok++;
				goto fmovem_loop_1;
			}

			if (*tok == KW_FPIAR)
			{
				regmask |= (1 << 10);
				tok++;
				goto fmovem_loop_1;
			}

			if ((*tok == '/') || (*tok == '-'))
			{
				tok++;
				goto fmovem_loop_1;
			}

			if (*tok++ != ',')
				return error("missing comma");

			if (amode(0) < 0)
				return OK;

			inst |= am0 | a0reg;
			D_word(inst);
			D_word(regmask);
			ea0gen(siz);
		}
		else
		{
			//fmovem.l ea,<rlist>
			if (amode(0) < 0)
				return OK;

			inst |= am0 | a0reg;

			if (*tok++ != ',')
				return error("missing comma");

			regmask = (1 << 15) | (0 << 13);

fmovem_loop_2:
			if (*tok == KW_FPCR)
			{
				regmask |= (1 << 12);
				tok++;
				goto fmovem_loop_2;
			}

			if (*tok == KW_FPSR)
			{
				regmask |= (1 << 11);
				tok++;
				goto fmovem_loop_2;
			}

			if (*tok == KW_FPIAR)
			{
				regmask |= (1 << 10);
				tok++;
				goto fmovem_loop_2;
			}

			if ((*tok == '/') || (*tok == '-'))
			{
				tok++;
				goto fmovem_loop_2;
			}

			if (*tok!=EOL)
				return error("extra (unexpected) text found");

			inst |= am0 | a0reg;
			D_word(inst);
			D_word(regmask);
			ea0gen(siz);
		}
	}
	else
		return error("bad size suffix");

	return OK;
}


//
// fmul (6888X, 68040)
//
int m_fmul(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00100011), FPU_P_EMUL);
}


int m_fsmul(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01100011), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


int m_fdmul(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01100111), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


//
// fneg (6888X, 68040)
//
int m_fneg(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00011010), FPU_P_EMUL);
}


int m_fsneg(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01011010), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


int m_fdneg(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01011110), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


//
// fnop (6888X, 68040)
//
int m_fnop(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00000000), FPU_P_EMUL);
}


//
// frem (6888X, 68040FPSP)
//
int m_frem(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00100101), FPU_FPSP);
}


//
// fscale (6888X, 68040FPSP)
//
int m_fscale(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00100110), FPU_FPSP);
}


//
// FScc (6888X, 68040)
//
int m_fseq  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00000001)); return OK;}
int m_fsne  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00001110)); return OK;}
int m_fsgt  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00010010)); return OK;}
int m_fsngt (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00011101)); return OK;}
int m_fsge  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00010011)); return OK;}
int m_fsnge (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00011100)); return OK;}
int m_fslt  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00010100)); return OK;}
int m_fsnlt (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00011011)); return OK;}
int m_fsle  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00010101)); return OK;}
int m_fsnle (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00011010)); return OK;}
int m_fsgl  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00010110)); return OK;}
int m_fsngl (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00011001)); return OK;}
int m_fsgle (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00010111)); return OK;}
int m_fsngle(WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00011000)); return OK;}
int m_fsogt (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00000010)); return OK;}
int m_fsule (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00001101)); return OK;}
int m_fsoge (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00000011)); return OK;}
int m_fsult (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00001100)); return OK;}
int m_fsolt (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00000100)); return OK;}
int m_fsuge (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00001011)); return OK;}
int m_fsole (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00000101)); return OK;}
int m_fsugt (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00001010)); return OK;}
int m_fsogl (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00000110)); return OK;}
int m_fsueq (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00001001)); return OK;}
int m_fsor  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00000111)); return OK;}
int m_fsun  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00001000)); return OK;}
int m_fsf   (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00000000)); return OK;}
int m_fst   (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00001111)); return OK;}
int m_fssf  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00010000)); return OK;}
int m_fsst  (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00011111)); return OK;}
int m_fsseq (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00010001)); return OK;}
int m_fssne (WORD inst, WORD siz) { inst|=am0|a0reg; D_word(inst); ea0gen(siz); D_word(B8(00011110)); return OK;}


//
// FTRAPcc (6888X, 68040)
//
int m_ftrapeq  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00000001)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00000001)); D_long(a0exval); } return OK;}
int m_ftrapne  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00001110)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00001110)); D_long(a0exval); } return OK;}
int m_ftrapgt  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00010010)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00010010)); D_long(a0exval); } return OK;}
int m_ftrapngt (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00011101)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00011101)); D_long(a0exval); } return OK;}
int m_ftrapge  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00010011)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00010011)); D_long(a0exval); } return OK;}
int m_ftrapnge (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00011100)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00011100)); D_long(a0exval); } return OK;}
int m_ftraplt  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00010100)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00010100)); D_long(a0exval); } return OK;}
int m_ftrapnlt (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00011011)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00011011)); D_long(a0exval); } return OK;}
int m_ftraple  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00010101)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00010101)); D_long(a0exval); } return OK;}
int m_ftrapnle (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00011010)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00011010)); D_long(a0exval); } return OK;}
int m_ftrapgl  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00010110)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00010110)); D_long(a0exval); } return OK;}
int m_ftrapngl (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00011001)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00011001)); D_long(a0exval); } return OK;}
int m_ftrapgle (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00010111)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00010111)); D_long(a0exval); } return OK;}
int m_ftrapngle(WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00011000)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00011000)); D_long(a0exval); } return OK;}
int m_ftrapogt (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00000010)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00000010)); D_long(a0exval); } return OK;}
int m_ftrapule (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00001101)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00001101)); D_long(a0exval); } return OK;}
int m_ftrapoge (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00000011)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00000011)); D_long(a0exval); } return OK;}
int m_ftrapult (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00001100)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00001100)); D_long(a0exval); } return OK;}
int m_ftrapolt (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00000100)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00000100)); D_long(a0exval); } return OK;}
int m_ftrapuge (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00001011)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00001011)); D_long(a0exval); } return OK;}
int m_ftrapole (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00000101)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00000101)); D_long(a0exval); } return OK;}
int m_ftrapugt (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00001010)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00001010)); D_long(a0exval); } return OK;}
int m_ftrapogl (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00000110)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00000110)); D_long(a0exval); } return OK;}
int m_ftrapueq (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00001001)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00001001)); D_long(a0exval); } return OK;}
int m_ftrapor  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00000111)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00000111)); D_long(a0exval); } return OK;}
int m_ftrapun  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00001000)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00001000)); D_long(a0exval); } return OK;}
int m_ftrapf   (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00000000)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00000000)); D_long(a0exval); } return OK;}
int m_ftrapt   (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00001111)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00001111)); D_long(a0exval); } return OK;}
int m_ftrapsf  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00010000)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00010000)); D_long(a0exval); } return OK;}
int m_ftrapst  (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00011111)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00011111)); D_long(a0exval); } return OK;}
int m_ftrapseq (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00010001)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00010001)); D_long(a0exval); } return OK;}
int m_ftrapsne (WORD inst, WORD siz) { if (siz==SIZW) { D_word(inst); D_word(B8(00011110)); D_word(a0exval); } else { inst|=3; D_word(inst); D_word(B8(00011110)); D_long(a0exval); } return OK;}

int m_ftrapeqn  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00000001)); return OK;}
int m_ftrapnen  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00001110)); return OK;}
int m_ftrapgtn  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00010010)); return OK;}
int m_ftrapngtn (WORD inst, WORD siz) { D_word(inst); D_word(B8(00011101)); return OK;}
int m_ftrapgen  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00010011)); return OK;}
int m_ftrapngen (WORD inst, WORD siz) { D_word(inst); D_word(B8(00011100)); return OK;}
int m_ftrapltn  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00010100)); return OK;}
int m_ftrapnltn (WORD inst, WORD siz) { D_word(inst); D_word(B8(00011011)); return OK;}
int m_ftraplen  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00010101)); return OK;}
int m_ftrapnlen (WORD inst, WORD siz) { D_word(inst); D_word(B8(00011010)); return OK;}
int m_ftrapgln  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00010110)); return OK;}
int m_ftrapngln (WORD inst, WORD siz) { D_word(inst); D_word(B8(00011001)); return OK;}
int m_ftrapglen (WORD inst, WORD siz) { D_word(inst); D_word(B8(00010111)); return OK;}
int m_ftrapnglen(WORD inst, WORD siz) { D_word(inst); D_word(B8(00011000)); return OK;}
int m_ftrapogtn (WORD inst, WORD siz) { D_word(inst); D_word(B8(00000010)); return OK;}
int m_ftrapulen (WORD inst, WORD siz) { D_word(inst); D_word(B8(00001101)); return OK;}
int m_ftrapogen (WORD inst, WORD siz) { D_word(inst); D_word(B8(00000011)); return OK;}
int m_ftrapultn (WORD inst, WORD siz) { D_word(inst); D_word(B8(00001100)); return OK;}
int m_ftrapoltn (WORD inst, WORD siz) { D_word(inst); D_word(B8(00000100)); return OK;}
int m_ftrapugen (WORD inst, WORD siz) { D_word(inst); D_word(B8(00001011)); return OK;}
int m_ftrapolen (WORD inst, WORD siz) { D_word(inst); D_word(B8(00000101)); return OK;}
int m_ftrapugtn (WORD inst, WORD siz) { D_word(inst); D_word(B8(00001010)); return OK;}
int m_ftrapogln (WORD inst, WORD siz) { D_word(inst); D_word(B8(00000110)); return OK;}
int m_ftrapueqn (WORD inst, WORD siz) { D_word(inst); D_word(B8(00001001)); return OK;}
int m_ftraporn  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00000111)); return OK;}
int m_ftrapunn  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00001000)); return OK;}
int m_ftrapfn   (WORD inst, WORD siz) { D_word(inst); D_word(B8(00000000)); return OK;}
int m_ftraptn   (WORD inst, WORD siz) { D_word(inst); D_word(B8(00001111)); return OK;}
int m_ftrapsfn  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00010000)); return OK;}
int m_ftrapstn  (WORD inst, WORD siz) { D_word(inst); D_word(B8(00011111)); return OK;}
int m_ftrapseqn (WORD inst, WORD siz) { D_word(inst); D_word(B8(00010001)); return OK;}
int m_ftrapsnen (WORD inst, WORD siz) { D_word(inst); D_word(B8(00011110)); return OK;}


//
// fsgldiv (6888X, 68040)
//
int m_fsgldiv(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00100100), FPU_P_EMUL);
}


//
// fsglmul (6888X, 68040)
//
int m_fsglmul(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00100111), FPU_P_EMUL);
}


//
// fsin (6888X, 68040FPSP)
//
int m_fsin(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00001110), FPU_FPSP);
}


//
// fsincos (6888X, 68040FPSP)
//
int m_fsincos(WORD inst, WORD siz)
{
	if (gen_fpu(inst, siz, B8(00110000), FPU_FPSP) == OK)
	{
		chptr[-1] |= a2reg;
		return OK;
	}
	else
		return ERROR;
}


//
// fsin (6888X, 68040FPSP)
//
int m_fsinh(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00000010), FPU_FPSP);
}


//
// fsqrt (6888X, 68040)
//
int m_fsqrt(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00000100), FPU_P_EMUL);
}


int m_fsfsqrt(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01000001), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


int m_fdfsqrt(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01000101), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


//
// fsub (6888X, 68040)
//
int m_fsub(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00101000), FPU_P_EMUL);
}


int m_fsfsub(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01101000), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


int m_fdsub(WORD inst, WORD siz)
{
	if (activefpu == FPU_68040)
		return gen_fpu(inst, siz, B8(01101100), FPU_P_EMUL);
	else
		return error("Unsupported in current FPU");
}


//
// ftan (6888X, 68040FPSP)
//
int m_ftan(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00001111), FPU_FPSP);
}


//
// ftanh (6888X, 68040FPSP)
//
int m_ftanh(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00001001), FPU_FPSP);
}


//
// ftentox (6888X, 68040FPSP)
//
int m_ftentox(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00010010), FPU_FPSP);
}


//
// ftst (6888X, 68040)
//
int m_ftst(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00111010), FPU_P_EMUL);
}


//
// ftwotox (6888X, 68040FPSP)
//
int m_ftwotox(WORD inst, WORD siz)
{
	return gen_fpu(inst, siz, B8(00010001), FPU_FPSP);
}

