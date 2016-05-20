/*
 * cat.c
 * Opens a file and print all its content onto the uart until EOF.
 * Created: 24.02.2016 21:31:55
 *  Author: Admin
 */ 
#include "../console.h"

#define CAT_BUFFERSIZE 64


void cat(char* args) {
	static char buf[CAT_BUFFERSIZE];
	
	utfs_fd fd;
		
	if (utfs_open_ex(&fd, currentDir(), args) != UTFS_OK) {
		writeString("no such file.\r\n");
		return;
	}

	if (tfts(fd.typesector) != et_file) {
		writeString("not a file.\r\n");
		return;
	}
	
	WORD read;
	while((read = utfs_read(&fd, (BYTE*)buf, CAT_BUFFERSIZE - 1)) != EOF) {
		buf[read] = 0;
		writeString(buf);
	}
	
	writeString("\r\n");
}
