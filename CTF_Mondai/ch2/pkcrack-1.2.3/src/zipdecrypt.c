/* 
 * zipdecrypt.c
 *
 * This file contains a function for decrypting a ZIP-archive with a given set of
 * key[012] values. It produces a ZIP-archive which can be unzipped using pkunzip
 * under DOS or unzip under UNIX.  I wrote this be cause stage 3 (password
 * generation) takes a couple of eons for finding long passwords.
 *
 * (C) 1996-2003,2024 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "pkctypes.h"
#include "crc.h"
#include "pkcrack.h"
#include "keystuff.h"
#include "headers.h"

extern int	read_sig( FILE *fp );
extern int	read_local( FILE *fp, char *fname, int flags );
extern int	read_dir( FILE *fp, int flags );
extern int	read_end( FILE *fp, int flags );
extern int	read_ddesc( FILE *fp, int flags );
extern int	write_sig( FILE *fp, unsigned int type );
extern int	write_local( FILE *fp, local *lh );
extern int	write_dir( FILE *fp, dirtype *dir );
extern int	write_end( FILE *fp, enddirtype *end );
extern int	write_ddesc( FILE *fp, ddesctype *ddesc );

#ifndef O_BINARY
#define	O_BINARY	0
#endif

#define	MIN(a,b)	(((a)<(b))?(a):(b))

#define	NUMOFFSETS	200

static uword	offset=0, dirOffset=0;

static struct file {
    char	*name;
    uword	relOffset;
} **offsets=NULL;

static int	numOffsets=0, maxOffsets;
static byte	buffer[1024];

static int copy( FILE *outf, FILE *inf, int len, int decrypt )
{
int	i, j, finished=0;
byte	p;

    len += 12*decrypt;

    i = fread( buffer, 1, MIN(1024,len), inf );
    if( i < MIN(1024,len) )
    {
	fprintf( stderr, "File too short. Aborting...\n" );
	finished = 1;
    }

    j = 0;
    if( decrypt ) {
        printf("Decrypting %s (",filename);
        for( ; j < 12; j++ ) {
            p = cUpdateKeys( buffer[j] );
            printf("%02x",p);
        }
	printf(")...");
        if( ( (lh.gpb[0]&0x08)==0x08 && p==lh.time[1] ) ||
            ( (lh.gpb[0]&0x08)==0    && p==MSB(lh.crc)) ) printf(" OK!\n");
        else printf(" FAILED?!\n");
    }

    while( len > 0 )
    {
	if( decrypt )
	    for( i = 0; i+j < MIN(1024,len); i++ )
		buffer[i+j] = cUpdateKeys( buffer[i+j] );
	fwrite( &buffer[j], 1, MIN(1024,len)-j, outf );
	offset += MIN(1024,len)-j;
	len -= MIN(1024,len);
	if( len > 0 )
	{
	    i = fread( buffer, 1, MIN(1024,len), inf );
	    if( i < MIN(1024,len) )
	    {
		fprintf( stderr, "File too short. Aborting...\n" );
		finished = 1;
	    }
	    j = 0;
	}
    }

    return finished;
}

int zipdecrypt( char *inname, char *outname, uword k0, uword k1, uword k2 )
{
int	i, finished=0, decrypt=0, err=0;
FILE	*infile, *outfile;

    if( !offsets ) {
	offsets = malloc( sizeof(struct file*) * NUMOFFSETS );
	if( !offsets ) {
	    fprintf( stderr, "ZipDecrypt: Out of memory!\n" );
	    exit( -1 );
	}
	maxOffsets = NUMOFFSETS;
    }

    infile = fopen( inname, "rb" );
    if( !infile )
    {
	fprintf( stderr, "ZipDecrypt: can't open input file %s!\n", inname );
	return 0;
    }
    outfile = fopen( outname, "wb" );
    if( !outfile )
    {
	fprintf( stderr, "ZipDecrypt: can't open output file %s!\n", outname );
	fclose( infile );
	return 0;
    }

    do{
	err = read_sig(infile);
	switch( err )
	{
	  case SIGN_LOCAL:
	    if( numOffsets >= maxOffsets ) {
		maxOffsets += NUMOFFSETS;
		offsets = realloc( offsets, sizeof(struct file*) * maxOffsets );
		if( !offsets ) {
		    fprintf( stderr, "Fatal error! Out of memory!\n" );
		    exit(1);
		}
	    }
	    offsets[numOffsets] = malloc( sizeof(struct file) );
	    if( !offsets[numOffsets] ) {
		fprintf( stderr, "Fatal error! Out of memory!\n" );
		exit(1);
	    }
	    offsets[numOffsets]->relOffset = offset;
	    err = !write_sig(outfile, SIGN_LOCAL);
	    offset += 4;
	    if( !err ) err = (read_local(infile,"",FLG_NO_SKIP)<0);
	    decrypt = lh.gpb[0]&1;
	    if( decrypt )
	    {
		lh.gpb[0] &= 0xfe;
		lh.csize -= 12;
		key0 = k0;
		key1 = k1;
		key2 = k2;
	    }
	    if( !err ) err = !write_local(outfile, &lh);
	    offset += 26+lh.flen;
	    if( !err && lh.extralen > 0 ) err = copy(outfile, infile, lh.extralen, 0);
	    offsets[numOffsets]->name = malloc( lh.flen+1 );
	    memcpy( offsets[numOffsets]->name, filename, lh.flen+1 );
	    numOffsets++;
	    if( !err )
		err = copy( outfile, infile, lh.csize, decrypt );

            if( !err && ((lh.gpb[0] & 0x08) == 0x08)) {
                /* Porcess Data Descriptor structure */
                err = (read_ddesc(infile, 0) < 0);
                if(!err) err = !write_ddesc(outfile, &ddesc);
                offset += (ddesc.signature ? 16 : 12);
            }
	    break;

	  case SIGN_DIR:
	    if( !dirOffset )
		dirOffset = offset;
	    err = !write_sig(outfile, SIGN_DIR);
	    offset += 4;
	    if( !err ) err = (read_dir(infile,FLG_NO_SKIP)<0);
	    if( dir.gpb[0]&1 )
	    {
		dir.gpb[0] &= 0xfe;
		dir.csize -= 12;
	    }
	    for( i = 0; !err && i < numOffsets && strcmp( offsets[i]->name, filename ); i++ );
	    if( i >= numOffsets )
	    {
		fprintf( stderr, "Central directory contains unknown filename: %s\n", filename );
		err = 1;
	    }
	    else
		dir.locoffset = offsets[i]->relOffset;
	    if( !err ) err = !write_dir( outfile, &dir );
	    offset += 42 + dir.flen;
	    if( !err && dir.extralen > 0 ) err = copy(outfile, infile, dir.extralen, 0);
	    if( !err && dir.commlen > 0 ) err = copy(outfile, infile, dir.commlen, 0);
	    break;

	  case SIGN_END:
	    err = !write_sig(outfile, SIGN_END);
	    offset += 4;
	    if( !err ) err = (read_end(infile,FLG_NO_SKIP)<0);
	    enddir.centroffset = dirOffset;
	    if( !err ) err = !write_end( outfile, &enddir );
	    offset += 18;
	    if( !err && enddir.commlen > 0 ) err = copy(outfile, infile, enddir.commlen, 0);
	    finished = 1;
	    break;

          case SIGN_DDESC:
	    fprintf( stderr, "Error: unexpected Data Descriptor structure (ZIP file may be corrupt)\n");
	    fclose( infile );
	    fclose( outfile );
	    return 0;
	    break;

	  default:
	    fprintf( stderr, "Error: unknown signature (ZIP file may be corrupt)\n");
	    fclose( infile );
	    fclose( outfile );
	    return 0;
	    break;
	}
    }while( !finished && !err );

    fclose( infile );
    fclose( outfile );

    return 1;
}
