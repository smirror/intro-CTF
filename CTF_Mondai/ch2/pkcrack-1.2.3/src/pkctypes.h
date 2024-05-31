/* 
 * pkctypes.h
 *
 * This header contains global type definitions for different
 * platforms.
 *
 * (C) 1996,1997,2003 by Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#ifndef _PKCTYPES_H
#define _PKCTYPES_H

#include <stdint.h>

#ifdef byte
#undef byte
#endif
#define	byte	uint8_t

#ifdef uword
#undef uword
#endif
#define uword	uint32_t

#ifdef ushort
#undef ushort
#endif
#define	ushort	uint16_t

#endif /* _PKCTYPES_H */
