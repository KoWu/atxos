/*
 * hardware.h
 *
 * Created: 05.11.2015 14:53:13
 *  Author: Admin
 */ 
#include <stdint.h>

#ifndef HARDWARE_H_
#define HARDWARE_H_

#define BYTE uint8_t
#define WORD uint16_t
#define DWORD uint32_t

#define LOBYTE(x) ((BYTE)((WORD)x))
#define HIBYTE(x) ((BYTE)(((WORD)x) >> 8))

#endif /* HARDWARE_H_ */
