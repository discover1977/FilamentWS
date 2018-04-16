/*
 * ВНИМАНИЕ!!!
 * Драйвер использует аппаратный SPI
 */
#ifndef _MAX72xx_
#define _MAX72xx_

#include <stdint.h>
#include "compilers_4.h"

/*********************** Настройки ***********************/
/* количество микросхем */
#define MAX72XX_NUMBERS		1

/* включение поддержки символов(прямой вывод) и функций вывода строк */
// 0 - подержка выключена
// 1 - поддержка включена
#define SYMBOL_INCLUDE		1

/* настройки SPI порта*/
#define SPI_PORT			PORTB
#define SPI_DDR				DDRB
#define SPI_MOSI			3
#define SPI_MISO			4
#define SPI_SCK				5
#define SPI_CS				2

/***************** Карта адресов MAX72xx *****************/
#define NO_OP			0x00
#define DIGIT_0			0x01
#define DIGIT_1			0x02
#define DIGIT_2			0x03
#define DIGIT_3			0x04
#define DIGIT_4			0x05
#define DIGIT_5			0x06
#define DIGIT_6			0x07
#define DIGIT_7			0x08
#define DECODE_MODE		0x09
#define INTENSITY		0x0A
#define SCAN_LIMIT		0x0B
#define SHUTDOWN		0x0C
#define DISPLAY_TEST	0x0F
/*********************************************************/

/****************** Code B Font MAX72xx ******************/
#define SYMBOL_MINUS	0x0A
#define SYMBOL_E		0x0B
#define SYMBOL_H		0x0C
#define SYMBOL_L		0x0D
#define SYMBOL_P		0x0E
#define SYMBOL_BLANK	0x0F
/*********************************************************/

/* пользовательские функции */

/***********************************************************************************
*   Function name :   MAX72xx_Init
*   Returns :         нет
*   Parameters :      intensity - интенсивность/яркость/ток сегмента,
*   Purpose :         инициализация микросхемы, начальное задание пареметров.
***********************************************************************************/
void MAX72xx_Init(uint8_t intensity);
/***********************************************************************************
*   Function name :   MAX72xx_Clear
*   Returns :         нет
*   Parameters :      ic_addr - адрес микросхемы
*   Purpose :         очистка индикатора
***********************************************************************************/
void MAX72xx_Clear(uint8_t ic_addr);
/***********************************************************************************
*   Function name :   MAX72xx_Send
*   Returns :         нет
*   Parameters :      ic_addr - адрес микросхемы,
*       			  reg - регистр MAX72xx,
*       			  data - данные,
*   Purpose :         передача данных на MAX72xx
***********************************************************************************/
void MAX72xx_Send(int8_t ic_addr, uint8_t reg, uint8_t data );
/***********************************************************************************
*   Function name :   MAX72xx_OutInt
*   Returns :         нет
*   Parameters :      ic_addr - адрес микросхемы/индикатора,
*       			  value - число,
*       			  comma_pos - позиция точки,
*   Purpose :         вывод знакового числа на индикатор по адресу
*   				  число выводится с начала строки
***********************************************************************************/
void MAX72xx_OutInt(uint8_t ic_addr, int32_t value, uint8_t comma_pos);
/***********************************************************************************
*   Function name :   MAX72xx_OutIntFormat
*   Returns :         нет
*   Parameters :      value - число,
*   				  start_pos - начальная позиция вывода числа
*   				  stop_pos - конечная позиция вывода числа
*       			  comma_pos - позиция точки,
*   Purpose :         Форматный вывод знакового числа на индикатор.
*   				  Вывод начинается с позиции start_pos по stop_pos.
*   				  При каскадировании микросхем - нумерация позиции скозная,
*   				  при двух последовательно соединённых микросхемах,
*					  индикатор/микросхема с адресом "0" имеет нумерацию разрядов 1-8,
*					  индикатор/микросхема с адресом "1" имеет нумерацию разрядов 9-16,
*					  и так далее.
***********************************************************************************/
void MAX72xx_OutIntFormat(int32_t value, uint8_t start_pos, uint8_t stop_pos, uint8_t comma_pos);
/***********************************************************************************
*   Function name :   MAX72xx_SetComma
*   Returns :         нет
*   Parameters :      pos - позиция,
*   				  val - значение: 1 - вывести точку, 0 - стереть,
*   Purpose :         Установка/стирание точки в позицию
***********************************************************************************/
void MAX72xx_SetComma(uint8_t pos, uint8_t val);
/***********************************************************************************
*   Function name :   MAX72xx_SetCommaMask
*   Returns :         нет
*   Parameters :      ic_addr - адрес микросхемы/индикатора,
*   				  comma_mask - маска,
*   Purpose :         Вывод точки/точек по маске
***********************************************************************************/
void MAX72xx_SetCommaMask(uint8_t ic_addr, uint8_t comma_mask);
void MAX72xx_SetIntensity(uint8_t ic_addr, uint8_t val);
#if SYMBOL_INCLUDE
/***********************************************************************************
*   Function name :   MAX72xx_OutSym
*   Returns :         нет
*   Parameters :      *str - указатель на строку,
*   				  pos - позиция,
*   Purpose :         Вывод символа/строки вправо начиная с указанной позиции.
***********************************************************************************/
void MAX72xx_OutSym(char *str, int8_t pos);
#endif

/***********************************************************
//BITS MACROS
//PASHGAN 2009
//CHIPENABLE.RU
//
//reg : имя переменной, регистра
//bit : позиция бита
//val : 0 или 1
************************************************************/
#define SUFIX	UL
#define Bit(bit)  (1<<(bit))

#define ClearBit(reg, bit)       reg &= (~(1UL<<(bit)))
//пример: ClearBit(PORTB, 1); //сбросить 1-й бит PORTB

#define SetBit(reg, bit)          reg |= (1UL<<(bit))
//пример: SetBit(PORTB, 3); //установить 3-й бит PORTB

#define SetBitVal(reg, bit, val) do{if ((val&1)==0) reg &= (~(1<<(bit)));\
                                  else reg |= (1<<(bit));}while(0)
//пример: SetBitVal(PORTB, 3, 1); //установить 3-й бит PORTB
//	  SetBitVal(PORTB, 2, 0); //сбросить 2-й бит PORTB

#define BitIsClear(reg, bit)    ((reg & (1<<(bit))) == 0)
//пример: if (BitIsClear(PORTB,1)) {...} //если бит очищен

#define BitIsSet(reg, bit)       ((reg & (1<<(bit))) != 0)
//пример: if(BitIsSet(PORTB,2)) {...} //если бит установлен

#define InvBit(reg, bit)	  reg ^= (1<<(bit))
//пример: InvBit(PORTB, 1); //инвертировать 1-й бит PORTB

#endif

