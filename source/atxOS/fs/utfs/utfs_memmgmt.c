/*
 * utfs_memmgmt.h
 * Memory related functionalities.
 * Created: 20.02.2016 15:15:20
 *  Author: Admin
 */ 

#include "utfs_memmgmt.h"
#include "../../driver/block/mmc.h"

//shared buffer
BYTE utfs_buffer[BLOCKSIZE];

/************************************************************************/
/* Convert a type and a sector into a 3 byte typesector.                */
/************************************************************************/
void to_typesector(utfs_entrytype type, DWORD sector, utfs_typesector* ts) {
	(type == et_dir) ? (sector |= 0x800000) : (sector &= 0x7fffff);
	ts->d[0] = (sector >> 16) & 0xff;
	ts->d[1] = (sector >> 8) & 0xff;
	ts->d[2] = (sector)& 0xff;
}

/************************************************************************/
/* Convert a 3 byte typesector into an entrytype and a sector.          */
/************************************************************************/
void from_typesector(utfs_typesector *ts, utfs_entrytype *type, DWORD *sector) {
	*sector = (DWORD)ts->d[0];
	*sector <<= 8;
	*sector |= ts->d[1];
	*sector <<= 8;
	*sector |= ts->d[2];

	if (type) {
		(*sector & 0x800000) ? (*type = et_dir) : (*type = et_file);
	}
	*sector &= ~(0x800000);
}

/************************************************************************/
/* Find a free sector with the help of a bitmap.                        */
/************************************************************************/
DWORD getFreeSector() {
	for (WORD s = 0; s < BITMAPSIZE; s++) {
		mmc_readsector(s, utfs_buffer);
		for (WORD i = 0; i < BLOCKSIZE; i++) {
			if (utfs_buffer[i] != 0xff) {
				BYTE bi;
				for (bi = 0; utfs_buffer[i] & 0x80 >> bi; bi++);
				utfs_buffer[i] |= 0x80 >> bi;
				mmc_writesector(s, utfs_buffer);
				return s * BLOCKSIZE * 8 + i * 8 + bi;
			}
		}
	}
	return -1;
}

/************************************************************************/
/* Free a sector and rewrite the bitmap.                                */
/************************************************************************/
void freeSector(DWORD sektor) {
	BYTE s;
	WORD i;
	BYTE bi;
	
	s = sektor / (BLOCKSIZE * 8);
	i = sektor % (BLOCKSIZE * 8);
	bi = i % 8;
	i = i / 8;
	
	mmc_readsector(s, utfs_buffer);
	utfs_buffer[i] &= ~(0x80 >> bi);
	mmc_writesector(s, utfs_buffer);
}

/************************************************************************/
/* Formats the memory fast by writing a new bitmap and a new root dir.  */
/************************************************************************/
void formatFsFast() {
	for (int n = 0; n < BLOCKSIZE; n++) {
		utfs_buffer[n] = 0;
	}
	for (int n = 1; n < BITMAPSIZE; n++) {
		mmc_writesector(n, utfs_buffer);
	}
	for (int n = 0; n < BITMAPSIZE / 8; n++) {
		utfs_buffer[n] = 0xff;
	}
	utfs_buffer[BITMAPSIZE / 8] = 0x80;
	mmc_writesector(0, utfs_buffer);


	for (int n = 0; n < BITMAPSIZE / 8; n++) {
		utfs_buffer[n] = 0;
	}
	utfs_dir *root = (utfs_dir*)utfs_buffer;
	to_typesector(et_dir, 0, &root->typesector_next);
	root->entries = 0;
	mmc_writesector(BITMAPSIZE, utfs_buffer);
}
