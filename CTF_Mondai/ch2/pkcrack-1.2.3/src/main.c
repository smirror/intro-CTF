/* 
 * main.c
 *
 * This file contains the main() function of the PkZip-cracker.
 * It reads the ciphertext and plaintext files and makes calls to the
 * actual cracking stages.
 *
 * (C) 1996-2003,2024 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <time.h>
#include <stdio.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "pkctypes.h"
#include "crc.h"
#include "mktmptbl.h"
#include "stage1.h"
#include "pkcrack.h"
#include "stage2.h"
#include "stage3.h"
#include "headers.h"

extern int zipdecrypt( char *infile,char *outfile, int k0, int k1, int k2 );

#ifndef O_BINARY
#define	O_BINARY	0
#endif

#define	GOODNUM		1000
#define	VERYGOOD	100

extern	byte	*extract(char*, char*, int caseflg);

byte	*plaintext, *ciphertext;
uword	*key2i;

static uword bestKey2i[GOODNUM];

int	numKey2s=0;

static int bestNum=GOODNUM;

int	bestOffset;

static void usage( char *myself )
{
    fprintf( stderr, "Usage: %s -c <crypted_file> -p <plaintext_file> [other_options],\n", myself );
    fprintf( stderr, "where [other_options] may be one or more of\n" );
    fprintf( stderr, " -o <offset>\tfor an offset of the plaintext into the ciphertext,\n\t\t\t(may be negative)\n" );
    fprintf( stderr, " -C <c-ZIP>\twhere c-ZIP is a ZIP-archive containing <crypted_file>\n" );
    fprintf( stderr, " -P <p-ZIP>\twhere p-ZIP is a ZIP-archive containing <plaintext_file>\n" );
    fprintf( stderr, " -d <d-file>\twhere d-file is the name of the decrypted archive which\n\t\twill be created by this program if the correct keys are found\n\t\t(can only be used in conjunction with the -C option)\n" );
    fprintf( stderr, " -i\tswitch off case-insensitive filename matching in ZIP-archives\n" );
    fprintf( stderr, " -a\tabort keys searching after first success\n" );
    fprintf( stderr, " -n\tno progress indicator\n" );
}

static void cleanup(void) {
    if (key2i) free(key2i);
    if (plaintext) free(plaintext);
    if (ciphertext) free(ciphertext);
}

int main( int argc, char **argv )
{
int		crypt = 0, plain = 0, cryptlength, plainlength;
struct stat	filestat;
time_t		now;
int		i, offset=12, caseflg=0, sabort=0, noprogress=0;
char		*cryptname=NULL, *plainname=NULL;
char		*cFromZIP=NULL, *pFromZIP=NULL, *decryptName=NULL;

    atexit(&cleanup);

    for( i = 1; i < argc; i++ )
    {
	if( !strcmp( "-o", argv[i] ) )
	{ /* offset */
	    offset += atoi( argv[++i] );
	}
	else if( !strcmp( "-c", argv[i] ) )
	{ /* ciphertext filename */
	    cryptname = argv[++i];
	}
	else if( !strcmp( "-p", argv[i] ) )
	{ /* plaintext filename */
	    plainname = argv[++i];
	}
	else if( !strcmp( "-C", argv[i] ) )
	{ /* ciphertext-ZIPfilename */
	    cFromZIP = argv[++i];
	}
	else if( !strcmp( "-P", argv[i] ) )
	{ /* plaintext-ZIPfilename */
	    pFromZIP = argv[++i];
	}
	else if( !strcmp( "-d", argv[i] ) )
	{ /* name of decrypted ZIP-archive */
	    decryptName = argv[++i];
	}
	else if( !strcmp( "-i", argv[i] ) )
	{ /* case-insensitive filename */
	    caseflg = FLG_CASE_SENSITIVE;
        }
        else if( !strcmp( "-a", argv[i] ) )
	{ /* abort searching on success */
	    sabort = 1;
        }
        else if( !strcmp( "-n", argv[i] ) )
	{ /* no progress indicator */
	    noprogress = 1;
        }
    }
    if( !cryptname || !plainname )
    {
	usage( argv[0] );
	exit(EXIT_FAILURE);
    }
    if( decryptName && !cFromZIP )
    {
	usage( argv[0] );
	exit(EXIT_FAILURE);
    }

    key2i = malloc( sizeof(uword)*KEY2SPACE );
    if( !key2i )
    {
	fprintf( stderr, "Sorry, not enough memory available.\n" );
	exit(EXIT_FAILURE);
    }

    if( !cFromZIP )
    {
	crypt = open( cryptname, O_RDONLY | O_BINARY );
	if( crypt == -1 )
	{
	    fprintf( stderr, "Cryptfile %s not found!\n", argv[1] );
	    exit(EXIT_FAILURE);
	}
	fstat( crypt, &filestat );
	cryptlength = filestat.st_size;
	ciphertext = malloc( cryptlength );
    }
    else
    {
	ciphertext = extract( cFromZIP, cryptname, caseflg );
	if( !ciphertext )
	    exit(EXIT_FAILURE);
	cryptlength = lh.csize;
    }

    if( !pFromZIP )
    {
	plain = open( plainname, O_RDONLY | O_BINARY );
	if( plain == -1 )
	{
	    fprintf( stderr, "Plaintextfile %s not found!\n", argv[2] );
	    exit(EXIT_FAILURE);
	}
	fstat( plain, &filestat );
	plainlength = filestat.st_size;
	plaintext = malloc( plainlength + offset );
    }
    else
    {
        byte *tmp;
	plaintext = extract( pFromZIP, plainname, caseflg );
	if( !plaintext )
	    exit(EXIT_FAILURE);
	plainlength = lh.csize;
        tmp = (byte*) malloc(plainlength + offset);
        if (!tmp) {
            exit(EXIT_FAILURE);
        }
        memcpy(tmp + offset, plaintext, plainlength);
        free(plaintext);
	plaintext = tmp;
    }

    if( plainlength > cryptlength-offset )
    {
	fprintf( stderr, "Warning! Plaintext is longer than Ciphertext!\n" );
    }
    if( plainlength < 13 )
    {
	fprintf( stderr, "Plaintext must be at least 13 bytes! Aborting.\n" );
	exit(EXIT_FAILURE);
    }

    cryptlength = plainlength + offset;
    if( !ciphertext || !plaintext )
    {
	fprintf( stderr, "Not enough memory!\n" );
	exit(EXIT_FAILURE);
    }
    if( !cFromZIP && read( crypt, ciphertext, cryptlength ) !=  cryptlength )
    {
	fprintf( stderr, "Couldn't read ciphertext!\n" );
	exit(EXIT_FAILURE);
    }
    if( !pFromZIP && read( plain, &plaintext[offset], plainlength ) != plainlength )
    {
	fprintf( stderr, "Couldn't read plaintext!\n" );
	exit(EXIT_FAILURE);
    }
    if( !pFromZIP )
	close( plain );
    if( !cFromZIP )
	close( crypt );

    now = time(NULL);
    fprintf( stderr, "Files read. Starting stage 1 on %s", ctime(&now) );

    preCompTemp();
    mkCrcTab();
    initMulTab();

    generate1stSetOfKey2s( cryptlength-1 );
    fprintf( stderr, "Now we're trying to reduce these...\n" );
    fflush( stderr );
    for( i = cryptlength-1; i >= offset+13 && numKey2s > 0 && bestNum > VERYGOOD; i-- )
    {
        if(!noprogress)
        {
            fprintf( stderr, "Reducing number of keys... %3.1f%%\r", 100.*(cryptlength-i)/(cryptlength-offset-13) );
/*            fflush( stderr ); */
        }
	reduceKey2s( i );
	if( numKey2s < bestNum )
	{
	    memcpy( bestKey2i, key2i, sizeof(uword)*numKey2s );
	    bestNum = numKey2s;
	    bestOffset = i-1;
	    fprintf( stderr, "Lowest number: %d values at offset %d\n", bestNum, bestOffset );
	    fflush( stderr );
	}
    }

    if( bestNum < GOODNUM )
    {
	memcpy( key2i, bestKey2i, sizeof(uword)*bestNum );
	numKey2s = bestNum;
    }
    else
	bestOffset = i;

    printf( "Done. Left with %d possible Values. bestOffset is %d.\n", numKey2s, bestOffset );
    fflush( stdout );

    now = time(NULL);
    fprintf( stderr, "Stage 1 completed. Starting stage 2 on %s", ctime(&now) );

    for( i = 0; i < numKey2s; i++ )
    {
        if(!noprogress)
        {
            fprintf( stderr, "Searching... %3.1f%%\r",100.*i/numKey2s);
/*            fflush( stderr ); */
        }
        buildKey2Lists( key2i[i], cryptlength-bestOffset, offset );
        if( sabort && got_keys ) break;
    }


    now = time(NULL);
    fprintf( stderr, "Stage 2 completed. Starting %s on %s",
		decryptName?"zipdecrypt":"password search", ctime(&now) );

    if( !got_keys )
	printf( "No solutions found. You must have chosen the wrong plaintext.\n" );
    else if( !decryptName )
	findPwd( loesung0, loesung1, loesung2 );
    else
	zipdecrypt( cFromZIP, decryptName, loesung0, loesung1, loesung2 );

    now = time(NULL);
    fprintf( stderr, "Finished on %s", ctime(&now) );

    return 0;
}
