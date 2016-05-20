/*
 * hardware.c
 *
 * Created: 22.02.2016 14:58:00
 *  Author: Admin
 */
#include "hardware.h"


int strcmp(const char* s1, const char* s2)
{
	while(*s1 && (*s1==*s2))
	s1++,s2++;
	return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

void* memcpy(void * dst, void const * src, unsigned int len)
{
	char * pDst = (char *) dst;
	char const * pSrc = (char const *) src;

	while (len--)
	{
		*pDst++ = *pSrc++;
	}

	return (dst);
}

void *memmove(void *s1, void *s2, unsigned int n)
{
	unsigned char *cs1 = s1, *cs2 = s2;

	if (cs1 == cs2)
		return s1;
	if (cs2 < cs1)
	{
		cs1 += n;
		cs2 += n;
		while (n--)
			*--cs1 = *--cs2;
	}
	else
		memcpy(s1, s2, n);
	return s1;
}

int strncmp(const char *s1, const char *s2, int n)
{
	for (; n > 0; s1++, s2++, --n) {
		if (*s1 != *s2) {
			return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
		}
		else if (*s1 == '\0') {
			return 0;
		}
	}
	
	return 0;
}

char* strncpy(char *s1, const char *s2, int n)
{
	char *s = s1;
	while (n > 0 && *s2 != '\0') {
		*s++ = *s2++;
		--n;
	}
	while (n > 0) {
		*s++ = '\0';
		--n;
	}
	return s1;
}

int atoi(char *str)
{
	int res = 0;
	
	for (int i = 0; str[i]; ++i)
	res = res*10 + str[i] - '0';
	
	return res;
}

int strlen(const char * str)
{
	const char *s;
	for (s = str; *s; ++s) {}
	return(s - str);
}

void reverse(char s[])
{
	int i, j;
	char c;
	
	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void itoa(int n, char s[])
{
	int i, sign;
	 
	if ((sign = n) < 0)
	n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	if (sign < 0)
	s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}
 