/* 
 * crc.h
 *
 * This file contains macros for computing CRC-checksums using a lookup-table
 * which has to be initialized using a function in crc.c
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#ifndef _CRC_H
#define _CRC_H

#define	crcword		unsigned int

extern void mkCrcTab(void);

extern crcword	crctab[256], crcinvtab[256];

#define CRCPOLY	0xedb88320

#define CRC32(x,c)	(((x)>>8)^crctab[((x)^(c))&0xff])
#define	INVCRC32(x,c)	(((x)<<8)^crcinvtab[((x)>>24)&0xff]^((c)&0xff))

#endif /* _CRC_H */
