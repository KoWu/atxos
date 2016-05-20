/*
 * mmc.h
 * MMC card driver.
 * Created: 19.02.2016 18:43:17
 *  Author: Admin
 */ 


#ifndef MMC_H_
#define MMC_H_

#include <avr/io.h>
#include "../../hardware.h"

#define CS_INIT()	(DDRB |= (1 << PORTB0))
#define CS_HI()		(PORTB |= (1 << PORTB0))
#define CS_LO()		(PORTB &= ~(1 << PORTB0))

#define SCK_INIT()	(DDRB |= (1 << PORTB1))
#define SCK_HI()	(PORTB |= (1 << PORTB1))
#define SCK_LO()	(PORTB &= ~(1 << PORTB1))
#define SCK_HILO()	do{SCK_HI(); SCK_LO();}while(0);

#define MOSI_INIT()	(DDRB |= (1 << PORTB2))
#define MOSI_HI()	(PORTB |= (1 << PORTB2))
#define MOSI_LO()	(PORTB &= ~(1 << PORTB2))

#define MISO_INIT() (DDRB &= ~(1 << PORTB3))
#define MISO_IN		(PINB & (1 << PORTB3))


#define CMD_RESET		0
#define CMD_INIT		1
#define CMD_R_SINGLE	17
#define CMD_W_SINGLE	24

#define R_MASK			0x0e
#define R_ACCEPTED		0x04
#define R_ERR_CRC		0x0a
#define R_ERR_WRITE		0x0c

#define TOKEN_START		0xfe

#define MMC_SECTOR_SIZE	512


/************************************************************************/
/* Initialization sequence for MMC cards.                               */
/************************************************************************/
BYTE mmc_initialize();

/************************************************************************/
/* Writes a buf of 512 bytes size to the desired sector number.         */
/************************************************************************/
WORD mmc_writesector(DWORD sector, BYTE* buf);

/************************************************************************/
/* Reads 512 bytes from the specified sector number into buf.           */
/************************************************************************/
WORD mmc_readsector(DWORD sector, BYTE* buf);

/************************************************************************/
/* Reads n bytes from the specified sector + offset into buf.           */
/************************************************************************/
WORD mmc_readsector_ex(DWORD sector, BYTE* buf, WORD offset, WORD n);

#endif /* MMC_H_ */