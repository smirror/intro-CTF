/* pkcrack - makekey.c
 *
 * (C) 1997 Mats Lofkvist <mal@aristotle.algonet.se>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pkcrack.h"
#include "keystuff.h"
#include "crc.h"

static void usage( char *name )
{
    fprintf( stderr, "Usage: %s <password>\n", name );
    exit(EXIT_FAILURE);
}

int main( int argc, char **argv )
{
char *	pwd;  
int	pwdLen, i;

    if( argc != 2 )
	usage( argv[0] );

    pwd = argv[1];
    pwdLen = strlen( pwd );

    mkCrcTab( );
    initkeys( );
    for( i = 0; i < pwdLen; i++ )
	updateKeys( pwd[i] );

    printf( "%08x %08x %08x\n", key0, key1, key2 );

    return 0;
}

