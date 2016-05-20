/*
 * console.c
 * A simple console.
 * Created: 20.02.2016 15:15:20
 *  Author: Admin
 */ 
#include "console.h"

//needed for initialization routine
#include "../atxOs/driver/block/mmc.h"

//console procs
#include "echo/echo.h"
#include "mkdir/mkdir.h"
#include "sleep/sleep.h"
#include "uptime/uptime.h"
#include "format/format.h"
#include "ls/ls.h"
#include "touch/touch.h"
#include "write/write.h"
#include "cat/cat.h"

//console stack
static BYTE stack[256];

//holds the current directory
static utfs_fd current_directory;

//console internal procs.
static void cd(char* args);

//a mapping of string to proc.
static stringfunc procs[] = {
	{"ls", ls},
	{"cd", cd},
	{"mkdir", mkdir},
	{"echo", echo},
	{"sleep", sleep},
	{"format", format},
	{"uptime", uptime},
	{"touch", touch},
	{"write", write},
	{"cat", cat},
	{0, 0}
};

//changes the current directory.
static void cd(char* args) {
	utfs_fd next_directory;
	
	if (utfs_open_ex(&next_directory, &current_directory, args) != UTFS_OK) {
		writeString("Unknown directory ");
		writeString(args);
		writeString("\r\n");
	} else if (tfts(next_directory.typesector) != et_dir) {
		writeString("Not a directory.\r\n");
	} else {
		current_directory = next_directory;
	}
}

//checks whether a char is in the allowed char range (no ctrl nor extended chars allowed).
static char validChar(char c) {
	if (c >= ' ' && c <= '~')
	return 1;
	else
	return 0;
}

/************************************************************************/
/* Prints a string over uart.                                           */
/************************************************************************/
void writeString(char *str)
{
	while (*str) {
		uart_putc(*str++);
	}
}

//reads a max 32 char string from uart, supporting delete chars.
static char* readString(void)
{
	static char rx[33];
	char tmp;
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

//checks if two commands match. Not possible to use strcmp because of whitespaces and following cmd args.
static int compareCmds(const char* s1, const char* s2)
{
	while(*s1 && (*s1 != ' ') && (*s1==*s2)) { s1++,s2++; }
	if (*s1 == ' ') {
		return *(const unsigned char*)s2;
	}
	return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

//returns a pointer to the first cmd argument.
static char* getFirstArg(char* cmd) {
	while(*cmd && *cmd != ' ') {cmd++;}
	while(*cmd && *cmd == ' ') {cmd++;}
	return cmd;
}

//executes a command with arguments.
static void runcmd(char* cmd) {
	for(stringfunc* f = procs; f->string; f++) {
		if (!compareCmds(cmd, f->string)) {
			f->func(getFirstArg(cmd));
			return;
		}
	}
	
	if (*cmd) {
		writeString("unknown command ");
		writeString(cmd);
		writeString("\r\n");
	}
}

//console thread.
void console() {
	char* r;
	
	//basic initialization of needed hw components.
	uart_initialize();
	mmc_initialize();
	utfs_open(&current_directory, "/");
	
	writeString("Hello!\r\n");
	for (;;) {
		writeString("> ");
		r = readString();
		writeString("\r\n");
		runcmd(r);
	}
}

/************************************************************************/
/* Returns the current working directory.                               */
/************************************************************************/
utfs_fd *currentDir() {
	return &current_directory;
}

/************************************************************************/
/* Registers the console (creates a thread).                            */
/************************************************************************/
void register_console() {
	CreateThread(console, stack, sizeof(stack), T_PRIO_HIGHEST);
}
