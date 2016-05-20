/*
 * format.c
 * Formats the mmc card. All data will be lost.
 * Created: 24.02.2016 19:23:09
 *  Author: Admin
 */ 
#include "../console.h"
#include "../../atxOS/fs/utfs/utfs_memmgmt.h"

void format(char* args) {
	writeString("sure? y/n");
	if (uart_getc() == 'y') {
		formatFsFast();
	}
	writeString("\r\n");
}