/* 
 * zdmain.c
 *
 * This file contains the main function of the zipdecrypt program.
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include "pkctypes.h"
#include "crc.h"
#include "pkcrack.h"
#include "keystuff.h"

extern void zipdecrypt( char*infile, char*outfile, int k0, int k1, int k2 );

int main( int argc, char **argv )
{
char *c;

    mkCrcTab();

    switch( argc ) {
    case 4:
        initkeys();
        for(c=argv[1];*c;c++) updateKeys(*c);
        break;
    case 6:
        sscanf(argv[1], "%x", &key0);
        sscanf(argv[2], "%x", &key1);
        sscanf(argv[3], "%x", &key2);
        break;
    default:
        fprintf( stderr, "Usage: %s {<password> | <key0> <key1> <key2>} <cryptedzipfile> <plainzipfile>\n", argv[0] );
        exit(EXIT_FAILURE);
    }

    zipdecrypt( argv[argc-2], argv[argc-1], key0, key1, key2 );

    return 0;
}
