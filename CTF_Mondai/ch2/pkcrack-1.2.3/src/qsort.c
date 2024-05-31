/* 
 * qsort.c
 *
 * This file implements the quicksort algorithm.
 *
 * (C) 1996-2004 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include "pkctypes.h"

extern uword *key2i;

/* Quicksort. Sort key2i[i..j]. */
void qSort(int i, int j) {
int	k, l;
uword	median, tmp;

    while( i < j-1 ) {
	median = key2i[(i+j)/2];
	for( k=i, l=j, key2i[(i+j)/2]=key2i[i]; k < l; )
	{
	    while( k < l && key2i[l] > median ) l--;
	    key2i[k] = key2i[l];
	    while( k < l && key2i[k] <= median ) k++;
	    key2i[l] = key2i[k];
	}
	key2i[k] = median;

	if( k-1-i < j-k-1 ) {
	    qSort( i, k-1 );
	    i = k+1;
	} else {
	    qSort( k+1, j );
	    j = k-1;
	}
    }

    if( i == j-1 && key2i[i] > key2i[j] ) {
	tmp = key2i[i];
	key2i[i] = key2i[j];
	key2i[j] = tmp;
    }
}
