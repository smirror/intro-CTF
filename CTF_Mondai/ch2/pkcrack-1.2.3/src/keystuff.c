/* 
 * keystuff.c
 *
 * This file contains functions for initializing and updating the
 * internal state of the PkZip cipher.
 *
 * (C) 1996-2003 Peter Conrad <conrad@unix-ag.uni-kl.de>
 */

#include "pkctypes.h"
#include "keystuff.h"
#include "pkcrack.h"
#include "crc.h"

uword	key0, key1, key2;
ushort	temp;
byte	key3;

void initkeys()
{
    key0 = KEY0INIT;
    key1 = KEY1INIT;
    key2 = KEY2INIT;
}

void updateKeys( byte plainbyte )
{
    key0 = CRC32( key0, plainbyte );
    key1 = (key1 + (key0&0xff))*CONST + 1;
    key2 = CRC32( key2, MSB(key1) );
    temp = (key2&0xffff) | 3;
    key3 = ((temp * (temp^1))>>8)&0xff;
}

byte cUpdateKeys( byte cipherbyte )
{
byte plainbyte;

    temp = (key2&0xffff) | 3;
    key3 = ((temp * (temp^1))>>8)&0xff;
    plainbyte = cipherbyte^key3;
    updateKeys( plainbyte );

    return plainbyte;
}

