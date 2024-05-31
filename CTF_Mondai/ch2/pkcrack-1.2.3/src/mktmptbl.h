/* 
 * mktmptbl.h
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#ifndef _MKTMPTBL_H
#define _MKTMPTBL_H

#include "pkctypes.h"
#include "pkcrack.h"

#define INVTEMP_NUM     5

extern void   preCompTemp(void);

extern ushort tempTable[256][64];
extern byte   invTempTable[256][64][INVTEMP_NUM];
extern ushort invEntries[256][64];

#define	MULT(x)		((mulTab[((x)>>24)&0xff]<<24) + \
			 (mulTab[((x)>>16)&0xff]<<16) + \
			 (mulTab[((x)>> 8)&0xff]<< 8) + \
			 (mulTab[((x)    )&0xff]    ))

extern uword	mulTab[256];
extern byte	mTab2[256][2];
extern byte	mTab2Counter[256];

extern void	initMulTab(void);

#endif /* _MKTMPTBL_H */
