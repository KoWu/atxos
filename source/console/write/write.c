/*
 * write.c
 * Writes into a file until CTRL+C is pressed.
 * Created: 24.02.2016 20:01:42
 *  Author: Admin
 */ 
#include "../console.h"

#define CTRL_C 3
#define CTRL_FF 12

#define WRITE_BUFFERSIZE 64 

void write(char* args) {
	static char buf[WRITE_BUFFERSIZE], *tmp, c;
	utfs_fd fd;
	
	if (utfs_open_ex(&fd, currentDir(), args) != UTFS_OK) {
		writeString("no such file.\r\n");
		return;
	}
	
	if (tfts(fd.typesector) != et_file) {
		writeString("not a file.\r\n");
		return;
	}
	
	uart_putc(CTRL_FF);
	
	tmp = buf;
	while ((c = uart_getc()) != CTRL_C) {
		uart_putc(c);
		*tmp++ = c;
		if (tmp == buf + WRITE_BUFFERSIZE) {
			utfs_write(&fd, (BYTE*)buf, WRITE_BUFFERSIZE);
			tmp = buf;
		}
	}
	utfs_write(&fd, (BYTE*)buf, (WORD)(tmp - buf));
	
	uart_putc(CTRL_FF);
}
