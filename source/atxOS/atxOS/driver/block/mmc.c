/*
 * mmc.c
 *
 * Created: 19.02.2016 18:43:30
 *  Author: Admin
 */
#include "mmc.h"
#include "../../time.h"

/************************************************************************/
/* Read n bytes from bus into buf.                                      */
/************************************************************************/
static void in(BYTE* buf, WORD n) {
	BYTE d;
	
	MOSI_HI();
	
	while(n--) {
		d = 0; if(MISO_IN) d++;
		SCK_HILO();
		d <<=1; if(MISO_IN) d++;
		SCK_HILO();
		d <<=1; if(MISO_IN) d++;
		SCK_HILO();
		d <<=1; if(MISO_IN) d++;
		SCK_HILO();
		d <<=1; if(MISO_IN) d++;
		SCK_HILO();
		d <<=1; if(MISO_IN) d++;
		SCK_HILO();
		d <<=1; if(MISO_IN) d++;
		SCK_HILO();
		d <<=1; if(MISO_IN) d++;
		SCK_HILO();
		*buf++ = d;
	}
}

/************************************************************************/
/* Read a single byte from bus.                                         */
/************************************************************************/
static BYTE bin() {
	BYTE r;
	in(&r, 1);
	return r;
}

/************************************************************************/
/* Write n bytes of data from buf on the bus.                           */
/************************************************************************/
static void out(BYTE* buf, WORD n) {
	BYTE d;
	
	while(n--) {
		d = *buf++;
		(d & 0x80) ? MOSI_HI() : MOSI_LO();
		SCK_HILO();
		(d & 0x40) ? MOSI_HI() : MOSI_LO();
		SCK_HILO();
		(d & 0x20) ? MOSI_HI() : MOSI_LO();
		SCK_HILO();
		(d & 0x10) ? MOSI_HI() : MOSI_LO();
		SCK_HILO();
		(d & 0x08) ? MOSI_HI() : MOSI_LO();
		SCK_HILO();
		(d & 0x04) ? MOSI_HI() : MOSI_LO();
		SCK_HILO();
		(d & 0x02) ? MOSI_HI() : MOSI_LO();
		SCK_HILO();
		(d & 0x01) ? MOSI_HI() : MOSI_LO();
		SCK_HILO();
	}
}

/************************************************************************/
/* Write a single byte on the bus.                                      */
/************************************************************************/
static void bout(BYTE b) {
	out(&b, 1);
}

/************************************************************************/
/* Perform n byte in/out operations discarding the result. bio = abfall */
/************************************************************************/
static void bio(BYTE n) {
	MOSI_HI();
	
	while(n--) {
		SCK_HILO();
		SCK_HILO();
		SCK_HILO();
		SCK_HILO();
		SCK_HILO();
		SCK_HILO();
		SCK_HILO();
		SCK_HILO();
	}
}

/************************************************************************/
/* Send a MMC command with arguments.                                   */
/************************************************************************/
static BYTE sendCmd(BYTE cmd, DWORD arg) {
	BYTE tmp, buf[6];
	
	buf[0] = 0x40 | cmd;
	buf[1] = (BYTE)(arg >> 24);
	buf[2] = (BYTE)(arg >> 16);
	buf[3] = (BYTE)(arg >> 8);
	buf[4] = (BYTE)arg;
	
	switch(cmd) {
		case CMD_RESET:
			tmp = 0x95;
			break;
		case CMD_INIT:
			tmp = 0xf9;
			break;
		default:
			tmp = 0x01;
	}
	buf[5] = tmp;
	
	while(bin() != 0xff);
	
	out(buf, 6);
	
	while((tmp = bin()) & 0x80);
	
	return tmp;
}

/************************************************************************/
/* Initialization sequence for MMC cards.                               */
/************************************************************************/
BYTE mmc_initialize() {
	BYTE tmp;
	
	CS_INIT();
	SCK_INIT();
	MOSI_INIT();
	MISO_INIT();
	
	{
		DWORD t = CurrentTime();
		if (t < 10) {
			Msleep(10 - t);
		}
	}
	
	CS_HI();
	SCK_LO();
	MOSI_LO();
	
	bio(10);

	CS_LO();

	if (sendCmd(CMD_RESET, 0) != 1) {
		return 1;
	}
	
	for(tmp = 100; tmp && sendCmd(CMD_INIT, 0); tmp--) {
		Usleep(100);
	}
	
	if (!tmp) {
		return 2;
	}
	
	return 0;
}

/************************************************************************/
/* Writes a buf of 512 bytes size to the desired sector number.         */
/************************************************************************/
BYTE mmc_writesector(DWORD sector, BYTE* buf) {
	sendCmd(CMD_W_SINGLE, sector * MMC_SECTOR_SIZE);
	bout(TOKEN_START);
	
	out(buf, MMC_SECTOR_SIZE);
	bio(2);
	
	if (((bin() & R_MASK) == R_ACCEPTED)) {
		bio(1);
		return 0;
	} else {
		bio(1);
		return -1;
	}
}

/************************************************************************/
/* Reads 512 bytes from the specified sector number into buf.           */
/************************************************************************/
BYTE mmc_readsector(DWORD sector, BYTE* buf) {
	sendCmd(CMD_R_SINGLE, sector * MMC_SECTOR_SIZE);
	while(bin() != TOKEN_START);
	
	in(buf, MMC_SECTOR_SIZE);
	bio(2);
	
	bio(1);
	return 0;
}
