//
// RMAC - Reboot's Macro Assembler for all Atari computers
// AMODE.C - Addressing Modes
// Copyright (C) 199x Landon Dyer, 2011-2017 Reboot and Friends
// RMAC derived from MADMAC v1.07 Written by Landon Dyer, 1986
// Source utilised with the kind permission of Landon Dyer
//

#include "amode.h"
#include "error.h"
#include "expr.h"
#include "mach.h"
#include "procln.h"
#include "rmac.h"
#include "sect.h"
#include "token.h"

#define DEF_KW
#include "kwtab.h"
#define DEF_MN
#include "mntab.h"


// Address-mode information
int nmodes;					// Number of addr'ing modes found
int am0;					// Addressing mode
int a0reg;					// Register
TOKEN a0expr[EXPRSIZE];		// Expression
VALUE a0exval;				// Expression's value
WORD a0exattr;				// Expression's attribute
int a0ixreg;				// Index register
int a0ixsiz;				// Index register size (and scale)
TOKEN a0oexpr[EXPRSIZE];	// Outer displacement expression
VALUE a0oexval;				// Outer displacement value
WORD a0oexattr;				// Outer displacement attribute
SYM * a0esym;				// External symbol involved in expr
TOKEN a0bexpr[EXPRSIZE];	// Base displacement expression
VALUE a0bexval;				// Base displacement value
WORD a0bexattr;				// Base displacement attribute
WORD a0bsize;				// Base displacement size
WORD a0extension;			// 020+ extension address word
WORD am0_030;				// ea bits for 020+ addressing modes

int am1;					// Addressing mode
int a1reg;					// Register
TOKEN a1expr[EXPRSIZE];		// Expression
VALUE a1exval;				// Expression's value
WORD a1exattr;				// Expression's attribute
int a1ixreg;				// Index register
int a1ixsiz;				// Index register size (and scale)
TOKEN a1oexpr[EXPRSIZE];	// Outer displacement expression
VALUE a1oexval;				// Outer displacement value
WORD a1oexattr;				// Outer displacement attribute
SYM * a1esym;				// External symbol involved in expr
TOKEN a1bexpr[EXPRSIZE];	// Base displacement expression
VALUE a1bexval;				// Base displacement value
WORD a1bexattr;				// Base displacement attribute
WORD a1bsize;				// Base displacement size
WORD a1extension;			// 020+ extension address word
WORD am1_030;				// ea bits for 020+ addressing modes

int a2reg;					// Register for div.l (68020+)
WORD mulmode;				// To distinguish between 32 and 64 bit multiplications (68020+)

int bfparam1;				// bfxxx instruction parameter 1
int bfparam2;				// bfxxx instruction parameter 2
TOKEN bf0expr[EXPRSIZE];	// Expression
VALUE bf0exval;				// Expression's value
WORD bf0exattr;				// Expression's attribute
SYM * bf0esym;				// External symbol involved in expr

// Function prototypes
int check030bf(void);


//
// Parse addressing mode
//
int amode(int acount)
{
	// Initialize global return values
	nmodes = a0reg = a1reg = 0;
	am0 = am1 = AM_NONE;
	a0expr[0] = a0oexpr[0] = a1expr[0] = a1oexpr[0] = ENDEXPR;
	a0exattr = a0oexattr = a1exattr = a1oexattr = 0;
	a0esym = a1esym = NULL;
	a0bexpr[0] = a1bexpr[0] = ENDEXPR;
	a0bexval = a0bsize = a0extension = a1bexval = a1bsize = a1extension = 0;
	am0_030 = am1_030 = 0;
	bfparam1 = bfparam2 = 0;
	bf0expr[0] = ENDEXPR;
	bf0exattr = 0;
	bf0esym = NULL;

	// If at EOL, then no addr modes at all
	if (*tok == EOL)
		return 0;

	// Parse first addressing mode
	#define AnOK      a0ok
	#define AMn       am0
	#define AnREG     a0reg
	#define AnIXREG   a0ixreg
	#define AnIXSIZ   a0ixsiz
	#define AnEXPR    a0expr
	#define AnEXVAL   a0exval
	#define AnEXATTR  a0exattr
	#define AnOEXPR   a0oexpr
	#define AnOEXVAL  a0oexval
	#define AnOEXATTR a0oexattr
	#define AnESYM    a0esym
	#define AMn_IX0   am0_ix0
	#define AMn_IXN   am0_ixn
	#define CHK_FOR_DISPn CheckForDisp0
	#define AnBEXPR   a0bexpr
	#define AnBEXVAL  a0bexval
	#define AnBEXATTR a0bexattr
	#define AnBZISE   a0bsize
	#define AnEXTEN   a0extension
	#define AMn_030   am0_030
	#define IS_SUPPRESSEDn IS_SUPPRESSED0
	#define CHECKODn CHECKOD0
	#include "parmode.h"

	// If caller wants only one mode, return just one (ignore comma);. If there
	// is no second addressing mode (no comma), then return just one anyway.
	nmodes = 1;

	// it's a bitfield instruction--check the parameters inside the {} block
	// for validity
	if ((*tok == '{') && (check030bf() == ERROR))
		return ERROR;

	if ((acount == 0) || (*tok != ','))
		return 1;

	// Eat the comma
	tok++;

	// Parse second addressing mode
	#define AnOK      a1ok
	#define AMn       am1
	#define AnREG     a1reg
	#define AnIXREG   a1ixreg
	#define AnIXSIZ   a1ixsiz
	#define AnEXPR    a1expr
	#define AnEXVAL   a1exval
	#define AnEXATTR  a1exattr
	#define AnOEXPR   a1oexpr
	#define AnOEXVAL  a1oexval
	#define AnOEXATTR a1oexattr
	#define AnESYM    a1esym
	#define AMn_IX0   am1_ix0
	#define AMn_IXN   am1_ixn
	#define CHK_FOR_DISPn CheckForDisp1
	#define AnBEXPR   a1bexpr
	#define AnBEXVAL  a1bexval
	#define AnBEXATTR a1bexattr
	#define AnBZISE   a1bsize
	#define AnEXTEN   a1extension
	#define AMn_030   am1_030
	#define IS_SUPPRESSEDn IS_SUPPRESSED1
	#define CHECKODn CHECKOD1
	#include "parmode.h"

	// It's a bitfield instruction--check the parameters inside the {} block
	// for validity
	if ((*tok == '{') && (check030bf() == ERROR))
		return ERROR;

	// At this point, it is legal for 020+ to have a ':'. For example divu.l
	// d0,d2:d3
	if (*tok == ':')
	{
		if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
			return error(unsupport);

		// TODO: protect this from combinations like Dx:FPx etc :)
		tok++;  //eat the colon

		if ((*tok >= KW_D0) && (*tok <= KW_D7))
		{
			a2reg = (*tok - KW_D0);
			mulmode = 1 << 10;
		}
		else if ((*tok >= KW_FP0) && (*tok <= KW_FP7))
		{
			a2reg = (*tok - KW_FP0);
			mulmode = 1 << 10;
		}
		else
			return error("a data or FPU register must follow a :");

		*tok++;
	}
	else
	{
		// If no ':' is present then maybe we have something like divs.l d0,d1
		// which sould translate to divs.l d0,d1:d1
		a2reg = a1reg;
		mulmode = 0;
	}

	nmodes = 2;
	return 2;

	// Error messages:
badmode:
	return error("addressing mode syntax");

	//unmode:
	//return error("unimplemented addressing mode");
}


//
// Parse register list
//
int reglist(WORD * a_rmask)
{
	static WORD msktab[] = {
		0x0001, 0x0002, 0x0004, 0x0008,
		0x0010, 0x0020, 0x0040, 0x0080,
		0x0100, 0x0200, 0x0400, 0x0800,
		0x1000, 0x2000, 0x4000, 0x8000
	};

	WORD rmask = 0;
	int r, cnt;

	for(;;)
	{
		if ((*tok >= KW_D0) && (*tok <= KW_A7))
			r = *tok++ & 0x0F;
		else
			break;

		if (*tok == '-')
		{
			tok++;

			if ((*tok >= KW_D0) && (*tok <= KW_A7))
				cnt = *tok++ & 0x0F;
			else
				return error("register list syntax");

			if (cnt < r)
				return error("register list order");

			cnt -= r;
		}
		else
			cnt = 0;

		while (cnt-- >= 0)
			rmask |= msktab[r++];

		if (*tok != '/')
			break;

		tok++;
	}

	*a_rmask = rmask;

	return OK;
}


//
// Parse FPU register list
//
int fpu_reglist_left(WORD * a_rmask)
{
	static WORD msktab_minus[] = {
		0x0080, 0x0040, 0x0020, 0x0010,
		0x0008, 0x0004, 0x0002, 0x0001
	};

	WORD rmask = 0;
	int r, cnt;

	for(;;)
	{
		if ((*tok >= KW_FP0) && (*tok <= KW_FP7))
			r = *tok++ & 0x07;
		else
			break;

		if (*tok == '-')
		{
			tok++;

			if ((*tok >= KW_FP0) && (*tok <= KW_FP7))
				cnt = *tok++ & 0x07;
			else
				return error("register list syntax");

			if (cnt < r)
				return error("register list order");

			cnt -= r;
		}
		else
			cnt = 0;

		while (cnt-- >= 0)
			rmask |= msktab_minus[r++];

		if (*tok != '/')
			break;

		tok++;
	}

	*a_rmask = rmask;

	return OK;
}


int fpu_reglist_right(WORD * a_rmask)
{
	static WORD msktab_plus[] = {
		0x0001, 0x0002, 0x0004, 0x0008,
		0x0010, 0x0020, 0x0040, 0x0080
	};

	WORD rmask = 0;
	int r, cnt;

	for(;;)
	{
		if ((*tok >= KW_FP0) && (*tok <= KW_FP7))
			r = *tok++ & 0x07;
		else
			break;

		if (*tok == '-')
		{
			tok++;

			if ((*tok >= KW_FP0) && (*tok <= KW_FP7))
				cnt = *tok++ & 0x07;
			else
				return error("register list syntax");

			if (cnt < r)
				return error("register list order");

			cnt -= r;
		}
		else
			cnt = 0;

		while (cnt-- >= 0)
			rmask |= msktab_plus[r++];

		if (*tok != '/')
			break;

		tok++;
	}

	*a_rmask = rmask;

	return OK;
}


//
// Check for bitfield instructions extra params
// These are 020+ instructions and have the following syntax:
// bfxxx <ea>{param1,param2}
// param1/2 are either data registers or immediate values
//
int check030bf(void)
{
	WARNING(Add more strict checks as well as checks for non defined (yet) labels)

	if ((activecpu & (CPU_68020 | CPU_68030 | CPU_68040)) == 0)
		return error(unsupport);

	tok++;

	if (*tok == CONST)
	{
		tok++;
		bfparam1 = *(int *)tok;

		if ((bfparam1 > 31) || (bfparam1 < 0))
			return error("bfxxx offset: immediate value must be between 0 and 31");

		// Do=0, offset=immediate - shift it to place
		bfparam1 = (bfparam1 << 6) | (0 << 11);
		tok++;
	}
	else if (*tok == SYMBOL)
	{
		if (expr(bf0expr, &bf0exval, &bf0exattr, &bf0esym) != OK)
			return ERROR;

		bfparam1 = bf0exval;

		if ((bfparam1 > 31) || (bfparam1 < 0))
			return error("bfxxx offset: immediate value must be between 0 and 31");

		// Do=0, offset=immediate - shift it to place
		bfparam1 = (bfparam1 << 6) | (0 << 11);
	}
	else if ((*tok >= KW_D0) && (*tok <= KW_D7))
	{
		// Do=1, offset=data register - shift it to place
		bfparam1 = ((*(int *)tok - 128) << 6) | (1 << 11);
		tok++;
	}
	else
		return ERROR;

	if (*tok==':')
		tok++;	//eat the ':'

	if (*tok == '}' && tok[1] == EOL)
	{
		// It is ok to have }, EOL here - it might be "fmove fpn,<ea> {dx}"
		tok++;
		return OK;
	}

	if (*tok == CONST)
	{
		tok++;
		bfparam2 = *(int *)tok;

		if (bfparam2 > 31 || bfparam2 < 0)
			return error("bfxxx width: immediate value must be between 0 and 31");

		// Do=0, offset=immediate - shift it to place
		bfparam2 = (bfparam2 << 0) | (0 << 5);
		tok++;
	}
	else if (*tok == SYMBOL)
	{
		if (expr(bf0expr, &bf0exval, &bf0exattr, &bf0esym) != OK)
			return ERROR;

		bfparam2 = bf0exval;

		if (bfparam2 > 31 || bfparam2 < 0)
			return error("bfxxx width: immediate value must be between 0 and 31");

		// Do=0, offset=immediate - shift it to place
		bfparam2 = (bfparam2 << 0) | (0 << 5);
	}
	else if ((*tok >= KW_D0) && (*tok <= KW_D7))
	{
		// Do=1, offset=data register - shift it to place
		bfparam2 = ((*(int *)tok - 128) << 0) | (1 << 5);
		tok++;
	}
	else
		return ERROR;

	tok++;	// Eat the '}'

	return OK;
}

