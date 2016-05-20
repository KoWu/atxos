/*
 * uptime.c
 * Prints the systems uptime.
 * Created: 24.02.2016 18:46:16
 *  Author: Admin
 */
#include "../console.h"

void uptime(char* args) {
	char buf[8];
	BYTE h;
	WORD s;
	DWORD m, time;
	
	time = CurrentTime();
	
	h = time / 3600000;
	m = time % 3600000;
	s = m % 60000;
	m = m / 60000;
	s = s / 1000;
	
	writeString("Uptime: ");
	itoa(h, buf);
	writeString(buf);
	writeString("h ");
	itoa(m, buf);
	writeString(buf);
	writeString("m ");
	itoa(s, buf);
	writeString(buf);
	writeString("s\r\n");
}