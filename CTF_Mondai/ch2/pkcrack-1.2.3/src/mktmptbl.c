/* 
 * mktmptbl.c
 *
 * This file contains a function for initializing a lookup-table that
 * is used for finding "temp" values for a given "key3" (refer to the paper if
 * you want to know what "temp" and "key3" are).
 *
 * (C) 1996-2003,2024 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <stdio.h>
#include <string.h>
#include "mktmptbl.h"

ushort	tempTable[256][64];
static int numEntries[256];

byte	invTempTable[256][64][INVTEMP_NUM];
ushort	invEntries[256][64];

void preCompTemp( )
{
uword	temp, key3;

    memset(numEntries, 0, sizeof(numEntries));
    memset(invEntries, 0, sizeof(invEntries));

    temp = 0x10000;
    do{
	key3 = (((temp|2) * (temp|3))>>8)&0xff;
	tempTable[key3][numEntries[key3]++] = temp;
/*	if( numEntries[key3] > 64 )
	    fprintf( stderr, "Warning! Lookup-table corrupted!\n" ); */
    }while( temp-=4 );

    /* invTempTable[key3][(anotherKey2Value>>10)&63] is to be used for finding
     * all k so that
     * tempTable[key3][k] & 0xfc00 == anotherKey2Value & 0xfc00 */
    for (key3 = 0; key3 < 256; key3++) {
	for (temp = 0; temp < 64; temp++) {
	    invTempTable[key3][(tempTable[key3][temp]>>10)&63]
			[invEntries[key3][(tempTable[key3][temp]>>10)&63]++] = temp;
/*	    if (invEntries[key3][(tempTable[key3][temp]>>10)&63] > INVTEMP_NUM){
		fprintf(stderr,"inventries != 1: %d\n", invEntries[key3][temp]);
		exit(-1);
	    } */
	}
    }
}

uword	mulTab[256];
byte	mTab2[256][2];
byte	mTab2Counter[256];

void initMulTab( )
{
uword	i, prod;
byte	j;

    memset(mTab2Counter, 0, sizeof(mTab2Counter));
    for( i = 0, prod = 0; i < 256; i++, prod+=INVCONST )
    {
	mulTab[i] = prod;
	j = MSB(prod);
	mTab2[j][mTab2Counter[j]++] = i;
    }
    mTab2[0xff][mTab2Counter[0xff]++] = 0;
}
