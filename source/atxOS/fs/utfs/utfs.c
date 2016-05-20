/*
 * utfs.c
 * A basic fs supporting well known I/O operations.
 * Created: 20.02.2016 15:15:20
 *  Author: Admin
 */ 

#include "../../hardware.h"
#include "../../driver/block/mmc.h"
#include "../../mutex.h"
#include "utfs.h"
#include "utfs_memmgmt.h"

//mutex making sure only one thread can access the fs at a time.
DEFINE_MUTEX(utfs_mutex);

//skip slashes until a new entry name begins.
static char* nextDirFileName(char* path) {
	while (*path && *path == '/') { path++; }
	return path;
}

//returns the name length of the following entry.
static BYTE getDirFileNameLength(char* path) {
	char *tmp = path;
	while (*tmp && *tmp != '/') { tmp++; }
	return (BYTE)(tmp - path);
}

/************************************************************************/
/* Opens the file or directory specified by path.                       */
/* root defines the root path, fd will be the returned file descriptor. */
/* On success UTFS_OK is returned.                                      */
/************************************************************************/
utfs_result utfs_open_ex(utfs_fd *fd, utfs_fd *root, char* path) {
	char name[MAXNAME + 1];
	BYTE len;

	//if the path is an absolute one, use the root directory as root.
	if (*path && *path == '/') {
		to_typesector(et_dir, BITMAPSIZE, &fd->typesector);
	} else {
		tscpy(&fd->typesector, &root->typesector);
	}

	MutexLock(&utfs_mutex);

	//go to the first entry name
	path = nextDirFileName(path);
	
	//while there are entries to loop through.
	while ((len = getDirFileNameLength(path)) > 0) {
		memcpy(name, path, min(len, MAXNAME));
		name[min(len, MAXNAME)] = 0;
		//only allow looping through directories.
		if (tfts(fd->typesector) != et_dir) {
			MutexUnlock(&utfs_mutex);
			return UTFS_NOT_A_DIR;
		}
		//check if the entry exists inside fd and overwrite fd with the found entry.
		if (getEntryTypesectorByName(&fd->typesector, name, &fd->typesector) != UTFS_OK) {
			MutexUnlock(&utfs_mutex);
			return UTFS_UNKNOWN_ENTRY;
		}
		//go to the next entry name.
		path = nextDirFileName(path + len);
	}
	
	MutexUnlock(&utfs_mutex);

	fd->seek = 0;
	return UTFS_OK;
}

/************************************************************************/
/* Like utfs_open_ex with the root directory as root parameter.         */
/************************************************************************/
utfs_result utfs_open(utfs_fd *fd, char* path) {
	utfs_fd root;
	to_typesector(et_dir, BITMAPSIZE, &root.typesector);
	return utfs_open_ex(fd, &root, path);
}

/************************************************************************/
/* Closes a file handle.                                                */
/************************************************************************/
utfs_result utfs_close(utfs_fd *fd) {
	return UTFS_OK;
}

/************************************************************************/
/* Writes n bytes from data into a file.                                */
/* Returns the amount of written bytes.                                 */
/************************************************************************/
WORD utfs_write(utfs_fd *fd, BYTE* data, WORD n) {
	utfs_file *file;

	if (tfts(fd->typesector) != et_file) {
		return 0;
	}

	MutexLock(&utfs_mutex);

	mmc_readsector(sfts(fd->typesector), utfs_buffer);
	file = (utfs_file*)utfs_buffer;

	//decide if we need to write a embedded or a multi sector file.
	if (file->header.type == ft_embedded) {
		n = writeEmbedded(fd, data, n);
	}
	else {
		n = writeMulti(fd, data, n);
	}

	MutexUnlock(&utfs_mutex);

	fd->seek += n;
	return n;
}

/************************************************************************/
/* Reads n bytes from file into data. Returns EOF on EOF or error.      */
/************************************************************************/
WORD utfs_read(utfs_fd *fd, BYTE* data, WORD n) {
	utfs_file *file;

	if (tfts(fd->typesector) != et_file) {
		return EOF;
	}

	MutexLock(&utfs_mutex);

	mmc_readsector(sfts(fd->typesector), utfs_buffer);
	file = (utfs_file*)utfs_buffer;

	//have we already reached EOF?
	if (fd->seek >= file->header.size) {
		MutexUnlock(&utfs_mutex);
		return EOF;
	}

	//decide if we need to deal with a embedded file or a multisektor file.
	if (file->header.type == ft_embedded) {
		n = readEmbedded(fd, data, n);
	}
	else {
		n = readMulti(fd, data, n);
	}

	MutexUnlock(&utfs_mutex);

	fd->seek += n;
	return n;
}

/************************************************************************/
/* Iterate through a directory. Returns UTFS_OK on success.             */
/* The callback must return a non zero value if other file operations   */
/* have been called inside the callback.                                */
/************************************************************************/
utfs_result utfs_enum(utfs_fd *fd, BYTE(*callback)(char* name, utfs_entrytype type)) {
	DWORD root;
	utfs_dir *dir;

	if (tfts(fd->typesector) != et_dir) {
		return UTFS_NOT_A_DIR;
	}

	dir = (utfs_dir*)utfs_buffer;
	root = sfts(fd->typesector);

	MutexLock(&utfs_mutex);

	do {
		mmc_readsector(root, utfs_buffer);
		for (BYTE n = 0; n < dir->entries; n++) {
			if (callback((char*)dir->entry[n].name, tfts(dir->entry[n].typesektor))) {
				//reread the sector because the buffer is invalid through the callback.
				mmc_readsector(root, utfs_buffer);
			}
		}

		root = sfts(dir->typesector_next);
	} while (root);

	MutexUnlock(&utfs_mutex);

	return UTFS_OK;
}

/************************************************************************/
/* Sets the seek position inside a file.                                */
/* SEEK_SET -> absolute, SEEK_CUR -> relative, SEEK_END -> at EOF.      */
/************************************************************************/
utfs_result utfs_seek(utfs_fd *fd, int pos, BYTE seektype) {
	if (tfts(fd->typesector) != et_file) {
		return UTFS_NOT_A_FILE;
	}
	
	switch (seektype)
	{
	case SEEK_SET:
		fd->seek = (WORD)pos;
		return UTFS_OK;
	case SEEK_CUR:
		fd->seek += pos;
		return UTFS_OK;
	case SEEK_END:
	{
		utfs_file *file = (utfs_file*)utfs_buffer;
		MutexLock(&utfs_mutex);
		mmc_readsector(sfts(fd->typesector), utfs_buffer);
		fd->seek = file->header.size + pos;
		MutexUnlock(&utfs_mutex);
		return UTFS_OK;
	}
	default:
		return UTFS_FUNCTION_MISUSE;
	}
}

/************************************************************************/
/* Returns the seek position of the current file.                       */
/************************************************************************/
WORD utfs_ftell(utfs_fd *fd) {
	return fd->seek;
}
