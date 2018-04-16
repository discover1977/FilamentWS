/*
 * weight_scales.h
 *
 *  Created on: 30 сент. 2017 г.
 *      Author: gavrilov.iv
 */

#ifndef WEIGHT_SCALES_H_
#define WEIGHT_SCALES_H_

#include <stdint.h>

//#define ZERO_SET_ERROR		0
#define SHIFT_HX711_DATA	0

void WSCALES_Init();
void WSCALE_SetZero(uint32_t ZeroValue);
uint32_t WSCALE_CalibrationZero(uint8_t average);
void WSCALES_SetCalibrationFactor(float value);
float WSCALES_Calibrate(uint16_t weight, uint8_t average);
float WSCALES_GetWeight(uint8_t average);
uint32_t WSCALES_GetADCData(uint8_t average);

#endif /* WEIGHT_SCALES_H_ */
