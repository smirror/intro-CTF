/* 
 * stage1.c
 *
 * This file implements stage 1 of the cracking process, namely finding
 * initial values for key2_n and reducing the number of possible values.
 * See sections 3.1 and 3.2 of the paper.
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include "stage1.h"
#include "crc.h"
#include "mktmptbl.h"
#include "pkcrack.h"
#include "qsort.h"

void generate1stSetOfKey2s( int n )
/* Generate all possible values of Key2_n and store them in key2[i] */
{
int	i, k;
byte	key3, key3a;
uword	j;
uword	aKey2Value, anotherKey2Value;

#ifdef DEBUG
uword	shouldBe;

    printf( "%5d %3d %3d ", n, ciphertext[n], plaintext[n] );
    scanf( "%x", &shouldBe );
    printf( "%8x\n", shouldBe );
#endif

    fprintf( stderr, "Generating 1st generation of possible key2_%d values...", n );
    fflush( stderr );

    numKey2s = 0;
    key3 = KEY3(n);
    key3a = KEY3(n-1);
    for( i = 0; i < 64; i++ )
	for( j = 0; j < 1<<16; j++ )
	{
/* See section 3.1 equation (1) of the Biham/Kocher paper. */
	    aKey2Value = tempTable[key3][i];
	    aKey2Value += (j<<16);
	    anotherKey2Value = INVCRC32(aKey2Value,0); /* rhs of eq. (1) */
	    /* find temp value for key3_{n-1} that fits rhs */
	    anotherKey2Value &= 0xfc00;
	    anotherKey2Value >>= 10;
	    for( k = 0; k < invEntries[key3a][anotherKey2Value]; k++ )
		key2i[numKey2s++] = aKey2Value |
				    (((tempTable[key3a]
						[invTempTable[key3a]
							     [anotherKey2Value]
							     [k]] ^
				       crcinvtab[MSB(aKey2Value)])>>8)&3);
	}

    fprintf( stderr, "done.\nFound %d possible key2-values.\n", numKey2s );
    fflush( stderr );
}

void reduceKey2s( int i )
/* For each possible key2_i generate key2_{i-1}, see section 3.2 of the paper.
 * Duplicate values are discarded. */
{
int	j, k, l, overhang = 0, newKeys=0;
uword	key2j, key2iminus1, key2iminus2, newKey;
byte	key3iminus1, key3iminus2;

#ifdef DEBUG
uword	shouldBe;

    printf( "%5d %3d %3d\n", i-1, ciphertext[i-1], plaintext[i-1] );
    scanf( "%x", &shouldBe );
    printf( "%8x\n", shouldBe );
#endif

    key3iminus1 = KEY3(i-1);
    key3iminus2 = KEY3(i-2);
    for( j = 0; j < numKey2s; j++ )
    {
	/* We change the equation to
	 * a) key2_{i-1}(31..8) = ((key2_i<<8)^crcinvtab[MSB(key2_i)]),
	 * b) key2_{i-1}(7..2)  = temp_{i-1}(7..2) for a temp_{i-1} that creates
						   key3_{i-1}
	 * c) key2_{i-1}(1,0)   = (temp_{i-2}^crcinvtab[MSB(key2_{i-1})])(9,8)
	      for a temp_{i-2} that creates key3_{i-2}
	 */
	key2j = key2i[j];
	key2iminus1 = INVCRC32(key2j,0)&0xffffff00; /* a) */
	for( k = 0; k < 64; k++ )
	    if ((tempTable[key3iminus1][k]&0xff00) == (ushort) (key2iminus1&0xff00))
	    {
		newKey = key2iminus1|tempTable[key3iminus1][k]; /* b) */
		key2iminus2 = INVCRC32(newKey,0)&0xfc00;
		key2iminus2 >>= 10;
		for( l = 0; l < invEntries[key3iminus2][key2iminus2]; l++ ) {
		    newKey = key2iminus1 |
			     (((tempTable[key3iminus2]
					 [invTempTable[key3iminus2]
						      [key2iminus2][l]] ^
				crcinvtab[MSB(key2iminus1)])>>8)&0xff); /* c) (and b) */
		    if( newKeys > j )
		        key2i[KEY2SPACE-(++overhang)] = newKey;
		    else
		        key2i[newKeys++] = newKey;
		    if( numKey2s + overhang >= KEY2SPACE )
		    {
		        fprintf( stderr, "Out of space for key2-values. Increase constants and recompile.\n" );
		        exit(1);
		    }
	        }
	    }
    }

    for( ; overhang > 0; overhang-- )
	key2i[newKeys++] = key2i[KEY2SPACE-overhang];
#ifdef DEBUG
    fprintf( stderr, "New number of possible key2i values: %d\n", newKeys );
    fflush( stderr );
#endif

    if( newKeys == 0 )
    {
	fprintf( stderr, "Argh! No more possible key2i values left!!!\n" );
	numKey2s = 0;
	return;
    }

    /* How to remove dupes efficiently */
    /* Sort all key2is using quicksort, condense result by skipping over dupes */
    qSort( 0, newKeys-1 );
    for( j = 1, numKey2s = 0; j < newKeys; j++ )
	if( key2i[j] != key2i[numKey2s] )
	    key2i[++numKey2s] = key2i[j];
    numKey2s++;
#ifdef DEBUG
    key2i[0] = shouldBe;
    numKey2s = 1;

    fprintf( stderr, "Removed %d dupes\n", newKeys-numKey2s );
    fflush( stderr );
#endif
}

