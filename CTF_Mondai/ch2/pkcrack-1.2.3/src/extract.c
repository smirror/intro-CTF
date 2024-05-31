/*
 * extract.c
 *
 * This file contains the main() function of a program for extracting
 * compressed data from a ZIP-archive. Gratefully received from a person who
 * wishes to remain anonymous.
 *
 * Slightly modified by Peter Conrad, Aug '96 - 2003, 2024
 *
 */
/******************************************************************************/
/*                                                                            */
/* FUNCTION       Extracts a file from a ZIP archive                          */
/*                                                                            */
/* INPUT          zipfilename                                                 */
/*                file to extract                                             */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#include <fcntl.h>
#include <string.h>

#include "headers.h"

extern char *extract( char *, char *, int caseflg );

#ifndef O_BINARY
#define	O_BINARY	0
#endif

static void usage( char *prg )
{
    fprintf( stderr, "Usage: %s [-i] [-p] [-v] [-s <size>] <zipfile> <file> [<outputfile>]\n", prg );
    fprintf( stderr, " -i\t\tswitch off case insensitive filename matching\n" );
    fprintf( stderr, " -p\t\tignore filepaths in archive\n" );
    fprintf( stderr, " -v\t\tbe verbose in parsing ZIP structure\n" );
    fprintf( stderr, " -s <size>\textract only specified number of bytes\n" );
}

int main(int argc, char *argv[])
{
char	*ret, *outname;
int	outfile, err=0, i, caseflg=0;
unsigned long	size=0;

    for( i = 1; i < argc; i++ ) {
    	if( !strcmp( "-i", argv[i] ) ) {
	    caseflg |= FLG_CASE_SENSITIVE;
	} else if( !strcmp( "-v", argv[i] ) ) {
	    caseflg |= FLG_VERBOSE;
	} else if( !strcmp( "-p", argv[i] ) ) {
	    caseflg |= FLG_IGNORE_PATH;
        } else if( !strcmp( "-s", argv[i] ) ) {
            if (sscanf(argv[++i],"%lu",&size) !=1 ) {
                fprintf(stderr,"Invalid <size>\n");
                exit(EXIT_FAILURE);
            }
        } else {
	    break;
	}
    }

    if( i+2 == argc ) {
	outname = argv[i+1];
    } else if( i+3 == argc ) {
	outname = argv[i+2];
    } else {
        usage( argv[0] );
        exit(EXIT_FAILURE);
    }

  /****************************************************************************/
  /* search file and write it to disk                                         */
  /****************************************************************************/
  err = !(ret = extract(argv[i], argv[i+1], caseflg ));

  if( !err )
  {
    outfile = open( outname, O_CREAT|O_WRONLY|O_BINARY, 0644 );
    if( outfile < 0 )
    {
	fprintf( stderr, "Couldn't create %s!\n", outname );
	err = 1;
    }
    else
    {
        if(size<=0) size = lh.csize;
        if(size>lh.csize) {
            fprintf(stderr,"<size> is too large, ignored\n");
            size = lh.csize;
        }
	while (size > INT_MAX && !err) {
            if( write( outfile, ret, INT_MAX ) != INT_MAX ) {
	        fprintf( stderr, "Couldn't write to %s!\n", outname );
	        err = 1;
	    }
	    ret += INT_MAX;
	}
        if (!err && write( outfile, ret, size ) != (int) size) {
	    fprintf( stderr, "Couldn't write to %s!\n", outname );
	    err = 1;
	}
	close( outfile );
    }
    free( ret );
  }
  if (err) { exit(EXIT_FAILURE); }

  return 0;
}
