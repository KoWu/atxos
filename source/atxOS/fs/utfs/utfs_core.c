/*
 * utfs_core.c
 * The core of the FS.
 * Created: 20.02.2016 15:15:20
 *  Author: Admin
 */ 

#include "../../hardware.h"
#include "../../driver/block/mmc.h"

#include "utfs_memmgmt.h"
#include "utfs_core.h"
#include "utfs.h"

/************************************************************************/
/* Allocates an entry inside a directory.                               */
/************************************************************************/
static DWORD allocDirEntry(DWORD root) {
	utfs_dir *dir = (utfs_dir*)utfs_buffer;

loopdir:
	mmc_readsector(root, utfs_buffer);

	//dir is full
	if (dir->entries == MAXDENTRY) {
		DWORD next_dir;
		from_typesector(&dir->typesector_next, 0, &next_dir);

		//a following dir has already been created.
		if (next_dir) {
			root = next_dir;
			goto loopdir;
		}

		//create a following dir.
		next_dir = getFreeSector();
		to_typesector(et_dir, next_dir, &dir->typesector_next);
		mmc_writesector(root, utfs_buffer);
		root = next_dir;
		dir->entries = 0;
		to_typesector(et_dir, 0, &dir->typesector_next);
	}

	return root;
}

/************************************************************************/
/* Create a directory inside the dir structure specified by root param. */
/************************************************************************/
DWORD mkDir(DWORD root, char* name) {
	utfs_dir *dir;
	utfs_dir_entry new_entry;
	DWORD new_dir;

	dir = (utfs_dir*)utfs_buffer;

	//allocate and write the new dir structure
	new_dir = getFreeSector();
	dir->entries = 0;
	to_typesector(et_dir, 0, &dir->typesector_next);
	mmc_writesector(new_dir, utfs_buffer);

	root = allocDirEntry(root);

	memcpy(new_entry.name, name, MAXNAME);
	to_typesector(et_dir, new_dir, &new_entry.typesektor);
	memcpy(&dir->entry[dir->entries++], &new_entry, sizeof(utfs_dir_entry));
	mmc_writesector(root, utfs_buffer);

	return new_dir;
}

/************************************************************************/
/* Searches for an entry name inside a dir and returns its typesector.  */
/************************************************************************/
utfs_result getEntryTypesectorByName(utfs_typesector *root, char* name, utfs_typesector *ts) {
	utfs_dir *dir = (utfs_dir*)utfs_buffer;
	utfs_entrytype type;
	DWORD sektor;

	//empty name? return root typesector.
	if (!*name) {
		tscpy(ts, root);
		return UTFS_OK;
	}

	from_typesector(root, &type, &sektor);
	
	//only directories allowed.
	if (type != et_dir) {
		return UTFS_NOT_A_DIR;
	}
	
	do {
		mmc_readsector(sektor, utfs_buffer);
		for (BYTE n = 0; n < dir->entries; n++) {
			if (!strncmp((char*)dir->entry[n].name, name, MAXNAME)) {
				tscpy(ts, &dir->entry[n].typesektor);
				return UTFS_OK;
			}
		}
		sektor = sfts(dir->typesector_next);
	} while (sektor);

	return UTFS_UNKNOWN_ENTRY;
}

/************************************************************************/
/* Creates a file inside the root sektor and returns a file descriptor. */
/************************************************************************/
utfs_result mkFil(utfs_fd *fd, DWORD root, char* name) {
	utfs_dir *dir;
	utfs_file *file;
	utfs_dir_entry new_entry;
	DWORD new_fil;

	file = (utfs_file*)utfs_buffer;

	//allocate and write the new (embedded) file structure
	new_fil = getFreeSector();
	file->header.type = ft_embedded;
	file->header.size = 0;
	mmc_writesector(new_fil, utfs_buffer);

	root = allocDirEntry(root);

	dir = (utfs_dir*)utfs_buffer;
	memcpy(new_entry.name, name, MAXNAME);
	to_typesector(et_file, new_fil, &new_entry.typesektor);
	memcpy(&dir->entry[dir->entries++], &new_entry, sizeof(utfs_dir_entry));
	mmc_writesector(root, utfs_buffer);

	//only if a handle is wanted.
	if (fd) {
		fd->seek = 0;
		to_typesector(et_file, new_fil, &fd->typesector);
	}

	return UTFS_OK;
}

/************************************************************************/
/* Writes a multi sektor file.                                          */
/************************************************************************/
WORD writeMulti(utfs_fd *fd, BYTE* data, WORD n) {
	utfs_file *file = (utfs_file*)utfs_buffer;
	BYTE si, ei;
	WORD sbi, ebi;
	DWORD rootsektor;

	si = fd->seek / MMC_SECTOR_SIZE;
	sbi = fd->seek % MMC_SECTOR_SIZE;
	ei = (fd->seek + n) / MMC_SECTOR_SIZE;
	ebi = (fd->seek + n) % MMC_SECTOR_SIZE;
	rootsektor = sfts(fd->typesector);

	//neue dateigröße updaten
	file->header.size = max(fd->seek + n, file->header.size);
	mmc_writesector(rootsektor, utfs_buffer);

	if (ei >= MAXFENTRY) {
		return 0;
	}

	//müssen neue sektoren registriert werden?
	if (ei >= file->multiblock.blocks) {
		for (BYTE c = ei - (file->multiblock.blocks - 1); c; c--) {
			DWORD sektor = getFreeSector();
			mmc_readsector(rootsektor, utfs_buffer);
			to_typesector(et_file, sektor, &file->multiblock.block[file->multiblock.blocks++]);
			mmc_writesector(rootsektor, utfs_buffer);
		}
	}

	//schreiben in einem sektor.
	if (si == ei) {
		DWORD sektor = sfts(file->multiblock.block[si]);
		mmc_readsector(sektor, utfs_buffer);
		memcpy(utfs_buffer + sbi, data, n);
		mmc_writesector(sektor, utfs_buffer);
		return n;
	}

	//schreiben über multiple sektoren erster sektor
	DWORD sektor = sfts(file->multiblock.block[si]);
	mmc_readsector(sektor, utfs_buffer);
	memcpy(utfs_buffer + sbi, data, MMC_SECTOR_SIZE - sbi);
	mmc_writesector(sektor, utfs_buffer);
	data += MMC_SECTOR_SIZE - sbi;

	//schreiben über multiple sektoren zwischensektoren
	for (BYTE c = 1; si + c < ei; c++) {
		mmc_readsector(rootsektor, utfs_buffer);
		sektor = sfts(file->multiblock.block[si + c]);
		mmc_writesector(sektor, data);
		data += MMC_SECTOR_SIZE;
	}

	//schreiben über multiple sektoren letzter sektor
	mmc_readsector(rootsektor, utfs_buffer);
	sektor = sfts(file->multiblock.block[ei]);
	mmc_readsector(sektor, utfs_buffer);
	memcpy(utfs_buffer, data, ebi);
	mmc_writesector(sektor, utfs_buffer);

	return n;
}

/************************************************************************/
/* Writes an embedded file.                                             */
/************************************************************************/
WORD writeEmbedded(utfs_fd *fd, BYTE *data, WORD n) {
	utfs_file *file = (utfs_file*)utfs_buffer;

	//too big for embedded? transform to multi.
	if (n > sizeof(utfs_file_embedded) || file->header.size + n > sizeof(utfs_file_embedded)) {
		DWORD new_sektor, size;

		//allocate new sector for the data
		new_sektor = getFreeSector();
		mmc_readsector(sfts(fd->typesector), utfs_buffer);
		size = file->header.size;
		memmove(utfs_buffer, file->embedded.data, size);
		mmc_writesector(new_sektor, utfs_buffer);

		//write a multiblock header
		file->header.type = ft_multiblock;
		file->header.size = size;
		file->multiblock.blocks = 1;
		to_typesector(et_file, new_sektor, &file->multiblock.block[0]);
		to_typesector(et_file, 0, &file->multiblock.nextblock);

		//let writeMulti do the actual work.
		return writeMulti(fd, data, n);
	}

	//copy n bytes from embedded file into the buffer
	memcpy(file->embedded.data + fd->seek, data, n);
	file->header.size = max(fd->seek + n, file->header.size);
	mmc_writesector(sfts(fd->typesector), utfs_buffer);
	return n;
}

/************************************************************************/
/* Reads a multi block file.                                            */
/************************************************************************/
WORD readMulti(utfs_fd *fd, BYTE* data, WORD n) {
	utfs_file *file = (utfs_file*)utfs_buffer;
	BYTE si, ei;
	WORD sbi, ebi;
	DWORD rootsektor;

	//partial read
	if (fd->seek + n > file->header.size) {
		n = file->header.size - fd->seek;
	}

	//calculate a starts/end sector index and a start/end byte index.
	si = fd->seek / MMC_SECTOR_SIZE;
	sbi = fd->seek % MMC_SECTOR_SIZE;
	ei = (fd->seek + n) / MMC_SECTOR_SIZE;
	ebi = (fd->seek + n) % MMC_SECTOR_SIZE;
	rootsektor = sfts(fd->typesector);

	//not supported currently. Would affect files > 83kb.
	if (ei > MAXFENTRY) {
		return 0;
	}

	//only one sector to read
	if (si == ei) {
		mmc_readsector(sfts(file->multiblock.block[si]), utfs_buffer);
		memcpy(data, utfs_buffer + sbi, n);
		return n;
	}

	//read first sector
	mmc_readsector(sfts(file->multiblock.block[si]), utfs_buffer);
	memcpy(data, utfs_buffer + sbi, MMC_SECTOR_SIZE - sbi);
	data += MMC_SECTOR_SIZE - sbi;

	//read sectors in between
	for (BYTE c = 1; si + c < ei; c++) {
		mmc_readsector(rootsektor, utfs_buffer);
		mmc_readsector(sfts(file->multiblock.block[si + c]), utfs_buffer);
		memcpy(data, utfs_buffer, MMC_SECTOR_SIZE);
		data += MMC_SECTOR_SIZE;
	}

	//read last sector
	mmc_readsector(rootsektor, utfs_buffer);
	mmc_readsector(sfts(file->multiblock.block[ei]), utfs_buffer);
	memcpy(data, utfs_buffer, ebi);

	return n;
}

/************************************************************************/
/* Reads an embedded file.                                              */
/************************************************************************/
WORD readEmbedded(utfs_fd *fd, BYTE *data, WORD n) {
	utfs_file *file = (utfs_file*)utfs_buffer;

	//partial read
	if (fd->seek + n > file->header.size) {
		n = file->header.size - fd->seek;
	}

	memcpy(data, file->embedded.data + fd->seek, n);
	return n;
}
