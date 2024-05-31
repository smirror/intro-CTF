/*
 * headers.h
 *
 * This file defines some structures containing information from a typical
 * ZIP-archive. Gratefully received from a person who wishes to remain 
 * anonymous. Some changes by Peter Conrad, August 1996 - 2003.
 */

#ifndef _HEADERS_H
#define _HEADERS_H

/******************************************************************************/
/* Various types and constants                                                */
/******************************************************************************/

/******************************************************************************/
/* Signatures constants / Header types                                        */
/******************************************************************************/
#define SIGN_LOCAL 0x04034b50u
#define SIGN_DIR   0x02014b50u
#define SIGN_END   0x06054b50u
#define SIGN_DDESC 0x08074b50u

/******************************************************************************
 * Flags								      *
 ******************************************************************************/
#define FLG_NO_SKIP		1
#define	FLG_CASE_SENSITIVE	2
#define FLG_VERBOSE		4
#define FLG_IGNORE_PATH         8

/******************************************************************************/
/* Local header                                                               */
/******************************************************************************/
typedef struct
{
  unsigned char version[2];
  unsigned char gpb[2];
  unsigned char compr[2];
  unsigned char time[2];
  unsigned char date[2];
  unsigned long crc;
  unsigned long csize;
  unsigned long uncsize;
  unsigned short flen;
  unsigned short extralen;
} local;

/******************************************************************************/
/* Directory                                                                  */
/******************************************************************************/
typedef struct
{
  unsigned char version[2];
  unsigned char verneed[2];
  unsigned char gpb[2];
  unsigned char compr[2];
  unsigned char time[2];
  unsigned char date[2];
  unsigned long crc;
  unsigned long csize;
  unsigned long uncsize;
  unsigned short flen;
  unsigned short extralen;
  unsigned short commlen;
  unsigned short disk;
  unsigned char attr[2];
  unsigned long extattr;
  unsigned long locoffset;
} dirtype;

/******************************************************************************/
/* End of directory                                                           */
/******************************************************************************/
typedef struct
{
  unsigned short diskno;
  unsigned short centrdiskno;
  unsigned short diskentries;
  unsigned short totentries;
  unsigned long centrsize;
  unsigned long centroffset;
  unsigned short commlen;
} enddirtype;

/******************************************************************************/
/* Data descriptor                                                            */
/******************************************************************************/
typedef struct
{
  unsigned long signature;
  unsigned long crc;
  unsigned long csize;
  unsigned long uncsize;
} ddesctype;

extern local		lh;
extern dirtype		dir;
extern enddirtype	enddir;
extern ddesctype	ddesc;
extern char		*filename;

#endif /* _HEADERS_H */
