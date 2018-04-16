/*
 * weight_scales.c
 *
 *  Created on: 30 сент. 2017 г.
 *      Author: gavrilov.iv
 */

#include "weight_scales.h"
#include "HX711.h"

static float CalibrationFactor = 0;
static int32_t Zero = 0;
static int32_t HX711Data = 0;


void WSCALES_Init(){
	HX711_Init();
	HX711_setGain(Gain_128);
}

uint32_t WSCALES_GetADCData(uint8_t average) {

	uint32_t summ = 0;
	if((average > 1) && (average <= 255)) {
	    for (uint8_t i = 0; i < average; i++)
	    {
	    	while(!HX711_IsReady());							// Waiting for data from HX711
	        summ += (HX711_readData() >> SHIFT_HX711_DATA);		// Reading data from HX711
	    }
	    HX711Data = summ / average;
	    return HX711Data;
	}
	else {
		while(!HX711_IsReady());
		HX711Data = HX711_readData() >> SHIFT_HX711_DATA;
		return HX711Data;
	}
}

void WSCALE_SetZero(uint32_t ZeroValue) {
	Zero = ZeroValue;
}

uint32_t WSCALE_CalibrationZero(uint8_t average){
	Zero = WSCALES_GetADCData(average);	// + ZERO_SET_ERROR;
	return Zero;
}

void WSCALES_SetCalibrationFactor(float value){
	CalibrationFactor = value;
}

float WSCALES_Calibrate(uint16_t weight, uint8_t average){
	CalibrationFactor = (float)((WSCALES_GetADCData(average) - Zero) / (float)weight);
	return CalibrationFactor;
}

float WSCALES_GetWeight(uint8_t average){
	WSCALES_GetADCData(average);
	return (float)(HX711Data - Zero) / CalibrationFactor;
}
