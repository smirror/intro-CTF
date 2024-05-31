/*
 * crc.c
 *
 * This file contains functions for calculating CRC-32 checksums.
 * The CRC-polynomial used is defined in crc.h
 * A lookup-table is used, which has to be initialized first.
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include "crc.h"

crcword crctab[256], crcinvtab[256];

void mkCrcTab( )
{
unsigned int i, j, c;

    for( i = 0; i < 256; i++ )
    {
	c = i;
	for( j = 0; j < 8; j++ )
	    if( c&1 )
		c = (c>>1) ^ CRCPOLY;
	    else
		c = (c>>1);
	crctab[i] = c;
	crcinvtab[c>>24] = (c<<8) ^ i;
    }
}

