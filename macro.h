////////////////////////////////////////////////////////////////////////////////////////////////////
// RMAC - Reboot's Macro Assembler for the Atari Jaguar Console System
// MACRO.H - Macro Definition and Invocation
// Copyright (C) 199x Landon Dyer, 2011 Reboot and Friends
// RMAC derived from MADMAC v1.07 Written by Landon Dyer, 1986
// Source Utilised with the Kind Permission of Landon Dyer

#ifndef __MACRO_H__
#define __MACRO_H__

#include "rmac.h"

// Globals, externals etc
extern LONG curuniq;
extern TOKEN **argp;
extern int mjump_align;

// Prototypes
void init_macro(void);
int exitmac(void);
int defmac(void);
int defrept(void);
int lncatch(int (*)(), char *);
int kwmatch(char *, char *);
int invokemac(SYM *, WORD);
void ib_macro(void);

#endif // __MACRO_H__

