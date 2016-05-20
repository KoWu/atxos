/*
 * utfs_core.h
 * The core of the FS.
 * Created: 20.02.2016 15:15:20
 *  Author: Admin
 */ 

#ifndef UTFS_CORE_H_
#define UTFS_CORE_H_

#include "../../hardware.h"

#define BITMAPSIZE 64
#define BLOCKSIZE 512
#define MAXNAME 13

//entrytypes of a directory.
typedef enum utfs_entrytype_e {
	et_file,
	et_dir
} utfs_entrytype;

//filetypes of a file.
typedef enum utfs_filetype_e {
	ft_embedded,
	ft_multiblock
} utfs_filetype;

//possible return values.
typedef enum utfs_result_e {
	UTFS_OK = 0,
	UTFS_UNKNOWN_ENTRY,
	UTFS_NOT_A_FILE,
	UTFS_NOT_A_DIR,
	UTFS_FUNCTION_MISUSE,
	UTFS_FS_ERROR,
	UTFS_NOT_EMPTY
} utfs_result;

//a typesector consists out of 3 bytes.
typedef struct __attribute__((__packed__)) utfs_typesector_s {
	BYTE d[3];
} utfs_typesector;

//a directory entry consists out of a type, a sector and a name.
typedef struct __attribute__((__packed__)) utfs_dir_entry_s {
	utfs_typesector typesektor;
	BYTE name[MAXNAME];
} utfs_dir_entry;

//the directory contains entries, the amount of entries it holds and a reference to a next dir struct.
typedef struct __attribute__((__packed__)) utfs_dir_s {
	BYTE entries;
	utfs_typesector typesector_next;
	BYTE unused[12];
	utfs_dir_entry entry[(BLOCKSIZE / sizeof(utfs_dir_entry)) - 1];
} utfs_dir;

//a file header consists out of the filetype and the total filesize.
typedef struct __attribute__((__packed__)) utfs_file_header_s {
	utfs_filetype type;
	DWORD size;
} utfs_file_header;

//an embedded file only holds user provided data.
typedef struct __attribute__((__packed__)) utfs_file_embedded_s {
	BYTE data[BLOCKSIZE - sizeof(utfs_file_header)];
} utfs_file_embedded;

//a multiblock file is build up similar to a directory.
typedef struct __attribute__((__packed__)) utfs_file_multiblock_s {
	BYTE blocks;
	utfs_typesector nextblock;
	BYTE unused[2];
	utfs_typesector block[(BLOCKSIZE - sizeof(utfs_file_header) - 6) / sizeof(utfs_typesector)];
} utfs_file_multiblock;

//a file contains a header and either an embedded file or multiblock data.
typedef struct __attribute__((__packed__)) utfs_file_s {
	utfs_file_header header;
	union {
		utfs_file_embedded embedded;
		utfs_file_multiblock multiblock;
	};
} utfs_file;

//a file descriptor holds the typesector of the file/dir and the current seek value.
typedef struct utfs_fd_s {
	utfs_typesector typesector;
	DWORD seek;
} utfs_fd;


//maximum directory entry.
#define MAXDENTRY ((BLOCKSIZE / sizeof(utfs_dir_entry)) - 1)

//maximum file entry.
#define MAXFENTRY ((BLOCKSIZE - sizeof(utfs_file_header) - 6) / sizeof(utfs_typesector))

/************************************************************************/
/* typesectorcopy. Copys a typesector.                                  */
/************************************************************************/
#define tscpy(to,from) do{(to)->d[0] = (from)->d[0]; (to)->d[1] = (from)->d[1]; (to)->d[2] = (from)->d[2];}while(0);

/************************************************************************/
/* sektorfromtypesector. Returns the sector of a typesector.            */
/************************************************************************/
#define sfts(ts) ((((DWORD)(ts.d[0] & 0x7f)) << 16) | (((DWORD)(ts.d[1])) << 8) | ((DWORD)(ts.d[2])))

/************************************************************************/
/* typefromtypesector. Returns the type of a typesector.                */
/************************************************************************/
#define tfts(ts) ((ts.d[0] & 0x80) ? (et_dir) : (et_file))

/************************************************************************/
/* Returns the bigger value.                                            */
/************************************************************************/
#define max(a,b) ((a > b) ? (a) : (b))

/************************************************************************/
/* Returns the smaller value.                                           */
/************************************************************************/
#define min(a,b) ((a < b) ? (a) : (b))


/************************************************************************/
/* Searches for an entry name inside a dir and returns its typesector.  */
/************************************************************************/
utfs_result getEntryTypesectorByName(utfs_typesector *root, char* name, utfs_typesector *ts);

/************************************************************************/
/* Creates a file inside the root sektor and returns a file descriptor. */
/************************************************************************/
utfs_result mkFil(utfs_fd *fd, DWORD root, char* name);

/************************************************************************/
/* Create a directory inside the dir structure specified by root param. */
/************************************************************************/
DWORD mkDir(DWORD root, char* name);

/************************************************************************/
/* Writes a multi sektor file.                                          */
/************************************************************************/
WORD writeMulti(utfs_fd *fd, BYTE* data, WORD n);

/************************************************************************/
/* Writes an embedded file.                                             */
/************************************************************************/
WORD writeEmbedded(utfs_fd *fd, BYTE *data, WORD n);

/************************************************************************/
/* Reads a multi block file.                                            */
/************************************************************************/
WORD readMulti(utfs_fd *fd, BYTE* data, WORD n);

/************************************************************************/
/* Reads an embedded file.                                              */
/************************************************************************/
WORD readEmbedded(utfs_fd *fd, BYTE *data, WORD n);

#endif