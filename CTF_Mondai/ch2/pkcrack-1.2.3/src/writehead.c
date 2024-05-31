/*
 * writehead.c
 *
 * This file contains functions for writing parts of a ZIP-archive.
 */

#include <stdio.h>
#include "headers.h"

static int write2int( FILE *fp, unsigned short src )
{
unsigned char	dummy[2];

    dummy[0] = src&0xff;
    dummy[1] = (src>>8)&0xff;

    if( fwrite( dummy, 2, 1, fp ) != 1 )
	return 0;

    return 1;
}

static int write4int( FILE *fp, unsigned long src )
{
    return (write2int( fp, src&0xffff ) && write2int( fp, (src>>16)&0xffff ));
}

int write_sig(FILE *fp, unsigned int sigType)
{
  switch(sigType)
  {
    case SIGN_LOCAL:
    case SIGN_DIR:
    case SIGN_END:
    case SIGN_DDESC:
      return write4int(fp,sigType);
      break;
  }
  fprintf( stderr, "Write unknown signature: 0x%08x\n", sigType );

  return -1;
}

int write_local(FILE *fp,local *lh)
{
int ret;

  ret = fwrite( lh->version, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( lh->gpb, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( lh->compr, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( lh->time, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( lh->date, 2, 1, fp );
  if( ret == 1 )
	ret = write4int( fp, lh->crc );
  if( ret == 1 )
	ret = write4int( fp, lh->csize );
  if( ret == 1 )
	ret = write4int( fp, lh->uncsize );
  if( ret == 1 )
	ret = write2int( fp, lh->flen );
  if( ret == 1 )
	ret = write2int( fp, lh->extralen );
  if(ret != 1)
  {
    printf("Error while writing local header\n");
    return(0);
  }
  else;

  ret = fwrite(filename, lh->flen, 1, fp);
  if(ret != 1)
  {
    printf("Error while writing filename\n");
    return(0);
  }
  else;

  return 1;
}

int write_dir(FILE *fp, dirtype *dir)
{
int ret;

  ret = fwrite( dir->version, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( dir->verneed, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( dir->gpb, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( dir->compr, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( dir->time, 2, 1, fp );
  if( ret == 1 )
	ret = fwrite( dir->date, 2, 1, fp );
  if( ret == 1 )
	ret = write4int( fp, dir->crc );
  if( ret == 1 )
	ret = write4int( fp, dir->csize );
  if( ret == 1 )
	ret = write4int( fp, dir->uncsize );
  if( ret == 1 )
	ret = write2int( fp, dir->flen );
  if( ret == 1 )
	ret = write2int( fp, dir->extralen );
  if( ret == 1 )
	ret = write2int( fp, dir->commlen );
  if( ret == 1 )
	ret = write2int( fp, dir->disk );
  if( ret == 1 )
	ret = fwrite( dir->attr, 2, 1, fp );
  if( ret == 1 )
	ret = write4int( fp, dir->extattr );
  if( ret == 1 )
	ret = write4int( fp, dir->locoffset );
  if(ret != 1)
  {
    printf("Error while writing dir header\n");
    return(0);
  }

  ret = fwrite(filename, dir->flen, 1, fp);
  if(ret != 1)
  {
    printf("Error while writing filename\n");
    return(0);
  }
  else;

  return 1;
}

int write_end(FILE *fp, enddirtype *enddir)
{
int ret;

  ret = write2int( fp, enddir->diskno );
  if( ret == 1 )
	ret = write2int( fp, enddir->centrdiskno );
  if( ret == 1 )
	ret = write2int( fp, enddir->diskentries );
  if( ret == 1 )
	ret = write2int( fp, enddir->totentries );
  if( ret == 1 )
	ret = write4int( fp, enddir->centrsize );
  if( ret == 1 )
	ret = write4int( fp, enddir->centroffset );
  if( ret == 1 )
	ret = write2int( fp, enddir->commlen );
  if(ret != 1)
  {
    printf("Error while writing end of dir\n");
    return(0);
  }

  return 1;
}

int write_ddesc(FILE *fp, ddesctype *ddesc)
{
int ret = 1;

  if(ddesc->signature) ret = write_sig(fp, SIGN_DDESC);
  if(ret == 1) ret = write4int( fp, ddesc->crc );
  if(ret == 1) ret = write4int( fp, ddesc->csize );
  if(ret == 1) ret = write4int( fp, ddesc->uncsize );

  if(ret != 1)
  {
    printf("Error while writing data descriptor\n");
    return(0);
  }

  return 1;
}
