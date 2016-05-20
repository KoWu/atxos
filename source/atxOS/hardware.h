/*
 * hardware.h
 * Basic definitions.
 * Created: 05.11.2015 14:53:13
 *  Author: Admin
 */ 
#include <stdint.h>

#include "driver/char/uart.h"

#ifndef HARDWARE_H_
#define HARDWARE_H_

#define BYTE uint8_t
#define WORD uint16_t
#define DWORD uint32_t

#define SBYTE int8_t
#define SWORD int16_t
#define SDWORD int32_t
#define SLWORD int64_t

#define LOBYTE(x) ((BYTE)((WORD)x))
#define HIBYTE(x) ((BYTE)(((WORD)x) >> 8))

/************************************************************************/
/* OS Function return values.                                           */
/************************************************************************/
typedef enum ATXVALUE_e {
	ATX_OK,
	ATX_ERR_NULLPOINTER,
	ATX_ERR_UNKNOWN
} ATXVALUE;


int strcmp(const char* s1, const char* s2);
void *memcpy(void * dst, void const * src, unsigned int len);
void *memmove(void *s1, void *s2, unsigned int n);
int strncmp(const char *s1, const char *s2, int n);
int atoi(char *str);
void itoa(int n, char s[]);
char* strncpy(char *s1, const char *s2, int n);

#endif /* HARDWARE_H_ */
