/*
 * HX711.h
 *
 *  Created on: 23 сент. 2017 г.
 *      Author: gavrilov.iv
 */

#ifndef HX711_H_
#define HX711_H_

#include <avr/io.h>

#define HX711_DATA_PORT 	PORTC
#define HX711_DATA_PIN	4			// Arduino Nano Pin - A4

#define HX711_SCK_PORT		PORTC
#define HX711_SCK_PIN	5			// Arduino Nano Pin - A5

enum Gain {
	Gain_128 = 1,
	Gain_32 = 2,
	Gain_64 = 3
};

#define DEFAULT_GAIN	Gain_128

void HX711_Init();
void HX711_PowerDown();
void HX711_setGain(uint8_t gainVal);
uint8_t HX711_IsReady();
uint32_t HX711_readData();

#endif /* HX711_H_ */
