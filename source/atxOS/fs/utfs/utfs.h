/*
 * utfs.h
 * A basic fs supporting well known I/O operations.
 * Created: 20.02.2016 15:15:20
 *  Author: Admin
 */ 

#ifndef UTFS_H_
#define UTFS_H_

#include "../../hardware.h"
#include "utfs_core.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define EOF -1

/************************************************************************/
/* Opens the file or directory specified by path.                       */
/* root defines the root path, fd will be the returned file descriptor. */
/* On success UTFS_OK is returned.                                      */
/************************************************************************/
utfs_result utfs_open_ex(utfs_fd *fd, utfs_fd *root, char* path);

/************************************************************************/
/* Like utfs_open_ex with the root directory as root parameter.         */
/************************************************************************/
utfs_result utfs_open(utfs_fd *fd, char* path);

/************************************************************************/
/* Closes a file handle.                                                */
/************************************************************************/
utfs_result utfs_close(utfs_fd *fd);

/************************************************************************/
/* Reads n bytes from file into data. Returns EOF on EOF or error.      */
/************************************************************************/
WORD utfs_read(utfs_fd *fd, BYTE* data, WORD n);

/************************************************************************/
/* Writes n bytes from data into a file.                                */
/* Returns the amount of written bytes.                                 */
/************************************************************************/
WORD utfs_write(utfs_fd *fd, BYTE* data, WORD n);

/************************************************************************/
/* Iterate through a directory. Returns UTFS_OK on success.             */
/* The callback must return a non zero value if other file operations   */
/* have been called inside the callback.                                */
/************************************************************************/
utfs_result utfs_enum(utfs_fd *fd, BYTE(*callback)(char* name, utfs_entrytype type));

/************************************************************************/
/* Sets the seek position inside a file.                                */
/* SEEK_SET -> absolute, SEEK_CUR -> relative, SEEK_END -> at EOF.      */
/************************************************************************/
utfs_result utfs_seek(utfs_fd *fd, int pos, BYTE seektype);

/************************************************************************/
/* Returns the seek position of the current file.                       */
/************************************************************************/
WORD utfs_ftell(utfs_fd *fd);

#endif