/* 
 * keystuff.h
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#ifndef _KEYSTUFF_H
#define _KEYSTUFF_H

#include "pkctypes.h"

#define KEY0INIT 0x12345678u
#define KEY1INIT 0x23456789u
#define KEY2INIT 0x34567890u

extern uword 	key0, key1, key2;
extern ushort	temp;
extern byte	key3;

extern void initkeys(void);
extern void updateKeys( byte plainbyte );
extern byte cUpdateKeys( byte cipherbyte );

#endif /* _KEYSTUFF_H */
