/*
 * utfs_memmgmt.h
 * Memory related functionalities.
 * Created: 20.02.2016 15:15:20
 *  Author: Admin
 */ 

#ifndef UTFS_MEMMGMT_H_
#define UTFS_MEMMGMT_H_

#include "../../hardware.h"
#include "utfs_core.h"

//A single shared buffer for all I/O operations.
extern BYTE utfs_buffer[BLOCKSIZE];

/************************************************************************/
/* Convert a type and a sector into a 3 byte typesector.                */
/************************************************************************/
void to_typesector(utfs_entrytype type, DWORD sector, utfs_typesector* ts);

/************************************************************************/
/* Convert a 3 byte typesector into an entrytype and a sector.          */
/************************************************************************/
void from_typesector(utfs_typesector *ts, utfs_entrytype *type, DWORD *sector);

/************************************************************************/
/* Find a free sector with the help of a bitmap.                        */
/************************************************************************/
DWORD getFreeSector();

/************************************************************************/
/* Free a sector and rewrite the bitmap.                                */
/************************************************************************/
void freeSector(DWORD sektor);

/************************************************************************/
/* Formats the memory fast by writing a new bitmap and a new root dir.  */
/************************************************************************/
void formatFsFast();

#endif