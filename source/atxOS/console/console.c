/*
 * console.c
 *
 * Created: 20.02.2016 15:15:20
 *  Author: Admin
 */ 
#include "console.h"

#include "on/on.h"
#include "off/off.h"

static BYTE stack[128];

stringfunc procs[] = {
	{"on", on},
	{"off", off},
	{0, 0}
};

int strcmp(const char* s1, const char* s2)
{
	while(*s1 && (*s1==*s2))
		s1++,s2++;
	return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

static char validChar(char c) {
	if (c >= ' ' && c <= '~')
	return 1;
	else
	return 0;
}

static void writeString(char *str)
{
	while (*str) {
		uart_putc(*str++);
	}
}

static char* readString(void)
{
	static char rx[33], tmp;
	BYTE i = 0;

	while((tmp = uart_getc()) != '\n' && tmp != '\r') {
		if (tmp == 0x7f) {
			if (i > 0) {
				rx[--i] = 0;
				uart_putc(tmp);
			}
			continue;
		}
		if (i < sizeof(rx) - 1) {
			if (validChar(tmp)) {
				rx[i++] = tmp;
				uart_putc(tmp);
			}
		}
	}
	rx[i] = 0;
	
	return rx;
}

static void runcmd(char* cmd) {
	for(stringfunc* f = procs; f->string; f++) {
		if (!strcmp(cmd, f->string)) {
			f->func();
			return;
		}
	}
	
	writeString("unknown cmd ");
	writeString(cmd);
	writeString("\r\n");
}

void console() {
	char* r;
	
	writeString("Hello!\r\n");
	for (;;) {
		writeString("> ");
		r = readString();
		writeString("\r\n");
		runcmd(r);
	}
}

void register_console() {
	uart_initialize();
	CreateThread(console, stack, sizeof(stack), T_PRIO_HIGHEST);
}
