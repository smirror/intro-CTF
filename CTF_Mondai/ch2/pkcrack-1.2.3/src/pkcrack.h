/* 
 * pkcrack.h
 *
 * This header file contains some constants used in the program and some
 * global variables from main.c
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#ifndef _PKCRACK_H
#define _PKCRACK_H

#include "pkctypes.h"

#define	KEY2SPACE	(1<<23)	/* some more just for safety */

#define	KEY3(i)	(plaintext[(i)]^ciphertext[(i)])

#define MSB(x)          (((x)>>24)&0xFF)

extern byte	*plaintext, *ciphertext;
extern uword	*key2i;
extern int	numKey2s, bestOffset;

#define	CONST		0x08088405U	/* 134775813 */
#define	INVCONST	0xd94fa8cdU	/* CONST^{-1} */

#define MAXDELTA        (0x00FFFFFFU+0xFFU)
#define MSBMASK         0xFF000000U

#endif /* _PKCRACK_H */
