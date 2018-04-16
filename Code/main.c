/*
 * main.c
 *
 *  Created on: 5 апреля. 2018 г.
 *      Author: gavrilov.iv
 */

// General inclusion
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

// User inclusion
#include "MAX72xx.h"
#include "HX711.h"
#include "weight_scales.h"
#include "encoder.h"
#include "buttons.h"

// Definitions
#define CALIBRATION_AVERAGE	25		// Количество выборок для усреднения калибровки
#define MES_AVERAGE	5				// Количество выборок для измерения веса

#define EXIT_DIGIT_INPUT	20000	// Таймаут для выхода из меню ввода числа
#define EXIT_TEST_ENCODER	10000	// Таймаут для выхода из меню тестирования энкодера и кнопки
#define EXIT_SETTING		20000	// Таймаут для выходы из меню настроек

#define EEPROM_INIT			0x01

// Menu items enumerator
enum EnumSettingItem {
	StartSetting,
		Calibration,
		CoilWeight,
		CalibrationZero,
		EncoderTest,
		EEPROMReset,
	EndSetting
};

// Global variables
volatile uint8_t ButtonCode, ButtonEvent, EncoderState;	// Button code and even variables. Encoder state variable
volatile uint16_t ExitCnt = 0;							// Timeout variable

// Struct EEPROM data
struct EEPROMData {
	uint8_t WSIsCalibrated;			// Flag - scales is calibrated
	float CalibrationFactor;		// Calibration coefficient value
	uint16_t CalibrationWeight;		// Weight of calibration standard value
	uint16_t CoilWeight;			// Coil weight value
	uint32_t WeightScaleZero;		// Weight "zero" value
} Param;

/*
 * Fetch button and event code
 */
void get_but() {
	ButtonCode = BUT_GetBut();
	if (ButtonCode) ButtonEvent = BUT_GetBut();
}

/*
 * Initializations EEPROM parameters
 */
void eeprom_init() {
	Param.CalibrationWeight = 1000;
	Param.CoilWeight = 0;
	Param.WeightScaleZero = 0;
}

/*
 * Save EEPROM parameters
 */
void save_eeprom() {
	cli();
	eeprom_update_block( (uint8_t*)&Param, 0, sizeof( Param ) );
	sei();
}

/*
 * Timer handler
 * Encoder poll
 * Button poll
 * Timeout timer realization
 */
ISR(TIMER0_COMPA_vect) {
	TCNT0 = 0x00;
	ENC_PollEncoder();
	BUT_Poll();
	if(ExitCnt) ExitCnt--;
}

/*
 * Button and encoder testing function
 */
void encoder_test() {
	MAX72xx_Clear(0);
	uint8_t Temp = 0;
	uint8_t LocalButEvent = 0, Display = 1;
	ExitCnt = EXIT_TEST_ENCODER;
	char Text[9];

	while(ExitCnt) {
		get_but();
		EncoderState = ENC_GetStateEncoder();

		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_RELEASED_CODE)) {
			ExitCnt = EXIT_TEST_ENCODER;
			LocalButEvent = ButtonEvent;
			Display = 1;
		}

		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_DOUBLE_CLICK_CODE)) {
			ExitCnt = EXIT_TEST_ENCODER;
			LocalButEvent = ButtonEvent;
			Display = 1;
		}

		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_RELEASED_LONG_CODE)) {
			ExitCnt = EXIT_TEST_ENCODER;
			LocalButEvent = ButtonEvent;
			Display = 1;
		}

		if(EncoderState == RIGHT_SPIN) {
			ExitCnt = EXIT_TEST_ENCODER;
			Temp++;
			Display = 1;
		}

		if(EncoderState == LEFT_SPIN) {
			ExitCnt = EXIT_TEST_ENCODER;
			Temp--;
			Display = 1;
		}

		if(Display == 1) {
			Display = 0;
			sprintf(Text, "b %d E%3d", LocalButEvent, Temp);
			MAX72xx_OutSym(Text, 8);
		}
	}
	MAX72xx_Clear(0);
}

/*
 * Function of entering numbers
 */
uint16_t input_digit(uint16_t InputValue, uint8_t NumberOfDigits) {
	int8_t InputValArray[NumberOfDigits], CommaPos = 1;
	uint16_t Temp = InputValue;
	uint16_t ReturnVal = InputValue;
	uint16_t Factor;
	ExitCnt = EXIT_DIGIT_INPUT;

	for(uint8_t i = 0; i < NumberOfDigits; i++) {
		InputValArray[i] = Temp % 10;
		Temp /= 10;
	}

	while(ExitCnt) {
		get_but();
		EncoderState = ENC_GetStateEncoder();

		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_RELEASED_CODE)) {
			if(++CommaPos > NumberOfDigits) CommaPos = 1;
			ExitCnt = EXIT_DIGIT_INPUT;
		}

		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_DOUBLE_CLICK_CODE)) {
			ReturnVal = Temp;
			ExitCnt = 0;
		}

		if(EncoderState == RIGHT_SPIN) {
			if(++InputValArray[CommaPos - 1] > 9) InputValArray[CommaPos - 1] = 0;
			ExitCnt = EXIT_DIGIT_INPUT;
		}

		if(EncoderState == LEFT_SPIN) {
			if(--InputValArray[CommaPos - 1] < 0) InputValArray[CommaPos - 1] = 9;
			ExitCnt = EXIT_DIGIT_INPUT;
		}

		Temp = 0;
		Factor = 1;
		for(uint8_t i = 0; i < NumberOfDigits; i++) {
			Temp += InputValArray[i] * Factor;
			Factor *= 10;
		}
		MAX72xx_OutIntFormat(Temp, 1, 4, CommaPos);
	}
	_delay_ms(1000);
	MAX72xx_Clear(0);
	return ReturnVal;
}

/*
 * Weight scales "null" calibration function
 */
void calibration_zero() {
	MAX72xx_OutSym("--------", 8);
	_delay_ms(1000);
	MAX72xx_OutSym(" ------ ", 8);
	_delay_ms(1000);
	MAX72xx_OutSym("  ----  ", 8);
	_delay_ms(1000);
	MAX72xx_OutSym("   --   ", 8);
	_delay_ms(1000);
	MAX72xx_OutSym("   ==   ", 8);
	Param.WeightScaleZero = WSCALE_CalibrationZero(CALIBRATION_AVERAGE);
	MAX72xx_OutSym("  donE  ", 8);
	_delay_ms(1000);
	MAX72xx_Clear(0);
}

/*
 * Weight scales calibration with standard
 */
void calibration() {
	calibration_zero();
	Param.CalibrationWeight = input_digit(Param.CalibrationWeight, 4);
	MAX72xx_Clear(0);
	ExitCnt = EXIT_SETTING + 1;

	while(ExitCnt) {
		get_but();

		if((ExitCnt % 1000) == 0) {
			MAX72xx_OutSym("CAncEL", 8);
			MAX72xx_OutIntFormat((ExitCnt / 1000), 1, 2, 0);
		}

		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_RELEASED_CODE)) {
			MAX72xx_OutSym("===CA===", 8);
			_delay_ms(1000);
			MAX72xx_OutSym(" ==CA== ", 8);
			_delay_ms(1000);
			MAX72xx_OutSym("  =CA=  ", 8);
			_delay_ms(1000);
			MAX72xx_OutSym("   CA   ", 8);
			_delay_ms(1000);
			MAX72xx_OutSym("CAL ProC", 8);
			Param.CalibrationFactor = WSCALES_Calibrate(Param.CalibrationWeight, CALIBRATION_AVERAGE);
			Param.WSIsCalibrated = 0x01;
			MAX72xx_OutSym("  donE  ", 8);
			_delay_ms(2000);
			MAX72xx_Clear(0);
			return;
		}

		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_DOUBLE_CLICK_CODE)) {
			ExitCnt = 0;
		}
	}
	if(ExitCnt == 0) {
		MAX72xx_OutSym("not donE", 8);
		_delay_ms(2000);
		MAX72xx_Clear(0);
		return;
	}
}

/*
 * Entering the weight of the coil
 */
void input_coil_weight() {
	Param.CoilWeight = input_digit(Param.CoilWeight, 3);
}

/*
 * Setting menu
 */
void setting() {
	int8_t SettingItem = Calibration;
	ExitCnt = EXIT_SETTING;
	char Text[5];
	MAX72xx_Clear(0);
	while(ExitCnt) {
		get_but();
		EncoderState = ENC_GetStateEncoder();

		sprintf(Text, "P-%2u", SettingItem);
		MAX72xx_OutSym(Text, 4);
		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_DOUBLE_CLICK_CODE)) {
			ExitCnt = 0;
		}

		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_RELEASED_CODE)) {
			ExitCnt = EXIT_SETTING;
			switch (SettingItem) {
				case Calibration: {
					calibration();
					Param.WSIsCalibrated = 1;
					ExitCnt = EXIT_SETTING;
					break;
				}
				case CoilWeight: {
					input_coil_weight();
					ExitCnt = EXIT_SETTING;
					break;
				}
				case CalibrationZero: {
					calibration_zero();
					ExitCnt = EXIT_SETTING;
					break;
				}
				case EncoderTest: {
					encoder_test();
					ExitCnt = EXIT_SETTING;
					break;
				}
				case EEPROMReset: {
					MAX72xx_OutSym("rESEt   ", 8);
				    uint8_t* Pointer = (uint8_t*)&Param;
				    for(int i = 0; i < sizeof(Param); i++) {
				    	*Pointer = 0xFF;
				    	Pointer++;
				    }
				    eeprom_init();
				    save_eeprom();
				    _delay_ms(1000);
				    MAX72xx_OutSym("donE    ", 8);
				    _delay_ms(1000);
					ExitCnt = EXIT_SETTING;
					MAX72xx_Clear(0);
					break;
				}
				default: break;
			}
		}

		if(EncoderState == RIGHT_SPIN) {
			if(++SettingItem == EndSetting) SettingItem = StartSetting + 1;
			ExitCnt = EXIT_SETTING;
		}

		if(EncoderState == LEFT_SPIN) {
			if(--SettingItem == StartSetting) SettingItem = EndSetting - 1;
			ExitCnt = EXIT_SETTING;
		}
	}
	save_eeprom();
	MAX72xx_Clear(0);
	_delay_ms(2000);
}

/*
 * Main function
 */
int main()
{
	volatile int32_t Weigth = 0;

	MAX72xx_Init(7);

	WSCALES_Init();

	BUT_Init();

	ENC_InitEncoder();

	SetBit(DDRD, 6);

    //  Timer 0 Initialization
	TCCR0A = 0x00;
	TCCR0B = ( 0 << CS02 ) | ( 1 << CS01 ) | ( 1 << CS00 );
    TCNT0 = 0x00;
    OCR0A = 0x7C;
    TIMSK0 = ( 1 << OCIE0A );

    // Read parameters from EEPROM
	eeprom_read_block( (uint8_t*)&Param, 0, sizeof( Param ) );

	// Checking EEPROM parameters
	if(Param.WSIsCalibrated == EEPROM_INIT) {
		WSCALES_SetCalibrationFactor(Param.CalibrationFactor);
		WSCALE_SetZero(Param.WeightScaleZero);
	}
	else {
		// Initializations EEPROM parameters
		eeprom_init();
		// Save EEPROM parameters
		save_eeprom();
	}

	sei();

	while(1) {
		// Fetch button and event code
		get_but();

		// Weight display if calibration is performed
		if(Param.WSIsCalibrated == EEPROM_INIT) {
			Weigth = (int32_t)WSCALES_GetWeight(MES_AVERAGE);
			MAX72xx_OutIntFormat(Weigth - Param.CoilWeight, 3, 8, 6);
		}
		else {
			MAX72xx_OutSym("________", 8);	// Underline display if calibration is not performed
		}

		// Вход в меню настройки
		if((ButtonCode == BUT_1_ID) && (ButtonEvent == BUT_RELEASED_LONG_CODE)) {
			setting();
		}
	}
}
