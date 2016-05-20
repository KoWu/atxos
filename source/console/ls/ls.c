/*
 * ls.c
 * Lists all entries in the current directory.
 * In case of files the file size is displayed, too.
 * Created: 24.02.2016 18:23:12
 *  Author: Admin
 */ 
#include "../console.h"

BYTE lscallback(char* name, utfs_entrytype type) {
	(type == et_file) ? (writeString("file   ")) : (writeString("dir    "));
	writeString(name);
	
	if (type == et_file) {
		utfs_fd file;
		char buffer[6];
		utfs_open_ex(&file, currentDir(), name);
		utfs_seek(&file, 0, SEEK_END);
		itoa(utfs_ftell(&file), buffer);
		utfs_close(&file);
		writeString("		");
		writeString(buffer);
		writeString("b\r\n");
		return 1;
	}
	
	writeString("\r\n");
	return 0;
}

void ls(char* args) {
	utfs_enum(currentDir(), lscallback);
}
