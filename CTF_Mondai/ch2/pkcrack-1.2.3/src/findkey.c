/* 
 * findkey.c
 *
 * This program tries to find a PkZip-password for a given initial state
 * of key0, key1 and key2. In the current version it prints information about
 * the progress of the search to stdout every couple of minutes. You can use
 * that information for resuming the search at a later time.
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include "pkctypes.h"
#include "pkcrack.h"
#include "stage3.h"
#include "crc.h"
#include "mktmptbl.h"

static void usage( char *name )
{
    fprintf( stderr, "Usage: %s <key0> <key1> <key2> [<pwdlen> <initvalue>]\n", name );
    fprintf( stderr, "<key0>, <key1> and <key2> must be in hexadecimal.\n" );
    fprintf( stderr, "<pwd> and <initvalue> can be given to continue an interrupted search.\n" );
    fprintf( stderr, "<initvalue> must also be in hexadecimal.\n" );
    exit(EXIT_FAILURE);
}

int main( int argc, char **argv )
{
uword	key0, key1, key2;
int	pwdLen=0;
uword	initBytes;

    if( argc != 4 && argc != 6 )
	usage( argv[0] );

    if( sscanf( argv[1], "%x", &key0 ) != 1 ||
	sscanf( argv[2], "%x", &key1 ) != 1 ||
	sscanf( argv[3], "%x", &key2 ) != 1 )
	usage( argv[0] );

    if( argc == 6 && (sscanf( argv[4], "%d", &pwdLen ) != 1 ||
		      sscanf( argv[5], "%x", &initBytes ) != 1) )
	usage( argv[0] );

    mkCrcTab( );
    initMulTab( );

    if( pwdLen > 0 )
	findLongPwd( key0, key1, key2, pwdLen, initBytes );
    else
	findPwd( key0, key1, key2 );

    return 0;
}
