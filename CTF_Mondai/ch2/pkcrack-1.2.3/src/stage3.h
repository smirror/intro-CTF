/* 
 * stage3.h
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#ifndef _STAGE3_H
#define _STAGE3_H

#include "pkctypes.h"

extern void findPwd( uword key0, uword key1, uword key2 );
extern void findLongPwd( uword key0, uword key1, uword key2, int pwdLen, uword initBytes );

#endif /* _STAGE3_H */
