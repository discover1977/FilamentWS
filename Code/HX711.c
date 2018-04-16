/*
 * HX711.c
 *
 *  Created on: 23 сент. 2017 г.
 *      Author: gavrilov.iv
 */

#include "HX711.h"
#include "bits_macros.h"

static uint8_t Gain;
static uint32_t Result = 0;
// static unsigned char *Port;
// static unsigned char *Pin;

// ћакросы дл€ доступа к регистрам порта
#define PortReg(port) (*(port))
#define DirReg(port) (*((port) - 1))
#define PinReg(port) (*((port) - 2))
#define DATA_PORT	BitIsSet(PinReg(&HX711_DATA_PORT), HX711_DATA_PIN)

static void __delay() {
	for(int i = 0; i < 1; i++) {
		asm("nop");
	}
}

/* HX711 port init */
void HX711_Init() {
	Gain = DEFAULT_GAIN;

	/* Port init */
	DirReg( &HX711_DATA_PORT ) |= ( 0 << HX711_DATA_PIN );
	PortReg( &HX711_DATA_PORT ) |= ( 1 << HX711_DATA_PIN );
	DirReg( &HX711_SCK_PORT ) |= ( 1 << HX711_SCK_PIN );
	PortReg( &HX711_SCK_PORT ) |= ( 0 << HX711_SCK_PIN );
}

void HX711_PowerDown() {
	SetBit(HX711_SCK_PORT, HX711_SCK_PIN);
}

/* HX711 set gain and input */
void HX711_setGain(uint8_t gainVal) {
	if ((gainVal > 0) && (gainVal <= Gain_64)) {
		Gain += gainVal;
	}
}

uint8_t HX711_IsReady() {
	ClearBit(HX711_SCK_PORT, HX711_SCK_PIN);
	if(DATA_PORT == 1) return 0;
	else return 1;
}

/* HX711_read data */
uint32_t HX711_readData() {

	ClearBit(HX711_SCK_PORT, HX711_SCK_PIN);
	__delay();
	Result = 0;

	for(int i = 0; i < 24; i++) {

		SetBit(HX711_SCK_PORT, HX711_SCK_PIN);
		Result = Result << 1;
		Result |= DATA_PORT;
		ClearBit(HX711_SCK_PORT, HX711_SCK_PIN);
	}

	for(uint8_t i = 1; i < Gain; i++) {
		SetBit(HX711_SCK_PORT, HX711_SCK_PIN);
		ClearBit(HX711_SCK_PORT, HX711_SCK_PIN);
	}
	Result = Result ^ 0x800000;
	return Result;
}
