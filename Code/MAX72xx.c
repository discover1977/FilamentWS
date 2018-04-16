#include "MAX72xx.h"

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>

#define IND_AMOUNT_NUM		8
#define ALL_AMOUNT_NUM		(IND_AMOUNT_NUM * MAX72XX_NUMBERS)

void MAX72xx_SendBuffer(uint8_t ic_addr);

uint8_t Buffer[ALL_AMOUNT_NUM];
volatile uint8_t SPI_Cmd;
volatile uint8_t SPI_Data;
volatile uint8_t MAX7219_Addr;
volatile uint64_t DecodeMask = 0xFFFFFFFFFFFFFFFF;

#if SYMBOL_INCLUDE

#define SEG_A 6
#define SEG_B 5
#define SEG_C 4
#define SEG_D 3
#define SEG_E 2
#define SEG_F 1
#define SEG_G 0
#define SEG_H 7

FLASH_VAR(uint8_t sym[]) =
{ /*символы*/
  '-', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '=', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '*', (1<<SEG_A)|(1<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '_', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  ' ', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),

  /*цифры*/
  '0', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  '1', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  '2', (1<<SEG_A)|(1<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '3', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '4', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '5', (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '6', (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '7', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  '8', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  '9', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  /*буквы*/
  'A', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'F', (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'b', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'U', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'u', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'S', (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'I', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'i', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(0<<SEG_E)|(0<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  't', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'H', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'E', (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'L', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'r', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'O', (1<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'o', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'P', (1<<SEG_A)|(1<<SEG_B)|(0<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'C', (1<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(0<<SEG_G)|(0<<SEG_H),
  'G', (1<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'c', (0<<SEG_A)|(0<<SEG_B)|(0<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'n', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'd', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(1<<SEG_E)|(0<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'h', (0<<SEG_A)|(0<<SEG_B)|(1<<SEG_C)|(0<<SEG_D)|(1<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
  'Y', (0<<SEG_A)|(1<<SEG_B)|(1<<SEG_C)|(1<<SEG_D)|(0<<SEG_E)|(1<<SEG_F)|(1<<SEG_G)|(0<<SEG_H),
   0
};

void MAX72xx_OutSym(char *str, int8_t pos)
{
	uint8_t i, j;
	uint8_t TmpMask = 0;
	char s;

	/*проверяем корректность заданной позиции*/
	if ((pos > ALL_AMOUNT_NUM) || (pos == 0)){
		return;
	}
	/*устанавливаем маску*/
	for (j = pos - strlen(str); j < pos; j++) {
		ClearBit(DecodeMask, j);
	}

	/*проходка по строке, пока она или индикатор не кончится*/
	for (j = pos - 1; j < ALL_AMOUNT_NUM; j--){
		s = *str++;
		if (!s){
			break;
		}

		/*поиск символа по таблице*/
		for (i = 0; read_byte_flash(sym[i]) != 0; i += 2) {
			if (s == read_byte_flash(sym[i])) {
				Buffer[j] = read_byte_flash(sym[i + 1]);
			}
		}
	}

	for (int i = 0; i < MAX72XX_NUMBERS; i++) {
		TmpMask = 0xFF & ((uint8_t)(DecodeMask >> (i * 8)));
		MAX72xx_Send(i, DECODE_MODE, TmpMask);
		MAX72xx_SendBuffer(i);
	}
}
#endif

void SPI_MasterInit(void)
{

	/* Set MOSI and SCK output, all others input */
	SetBit(SPI_PORT, SPI_CS);
	SetBit(SPI_PORT, SPI_MISO);
	SPI_DDR = (1 << SPI_MOSI) | (0 << SPI_MISO) | (1 << SPI_SCK) | (1 << SPI_CS);

	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = 0;
	SPCR = (0<<SPIE)|(1 << SPE)|(0 << DORD)|(1 << MSTR)|(0 << SPR1)|(0 << SPR0)|(0<<CPOL)|(0<<CPHA);
	SPSR = 0;
	SPSR = (0<<SPI2X);
}

void MAX72xx_Init(uint8_t intensity)
{
	if (intensity > 15) intensity= 5;
	SPI_MasterInit();
	for (int i = 0; i < MAX72XX_NUMBERS; i++) {
		MAX72xx_Send(i, DISPLAY_TEST, 0x00);
		MAX72xx_Send(i, SCAN_LIMIT, 0x07);
		// Decode mode - 0
		MAX72xx_Send(i, INTENSITY, (0x0F & intensity));
		MAX72xx_Clear(i);
		MAX72xx_Send(i, SHUTDOWN, 0x01);
	}
}

void MAX72xx_Send (int8_t ic_addr, uint8_t reg, uint8_t data)
{
	uint8_t spidata[ALL_AMOUNT_NUM];

	if (ic_addr >= MAX72XX_NUMBERS) {
		return;
	}

    // Create an array with the data to shift out
    int offset=ic_addr * 2;
    int maxbytes = MAX72XX_NUMBERS * 2;

    for(int i=0; i < maxbytes; i++)
	spidata[i] = 0;
    // put our device data into the array
    spidata[offset+1] = reg;
    spidata[offset] = data;
    // enable the line
    ClearBit(SPI_PORT, SPI_CS);
    // Now shift out the data
    for(int i = maxbytes; i > 0; i--) {
    	SPDR = spidata[i-1];
    	while(!(SPSR & (1<<SPIF)));
    }
    //latch the data onto the display
    SetBit(SPI_PORT, SPI_CS);
}

uint8_t make_number(int32_t value, uint8_t start_pos, uint8_t stop_pos, uint8_t comma_pos)
{
	volatile int32_t tmpVal;
	if (value < 0) {
		tmpVal = abs(value);
	}
	else {
		tmpVal = value;
	}

	int8_t ZnakPos = 1;

	/*
    for(i = 0; i < amount; i++){
       tmp = value % 10;
       if ((i == 0)||(value > 0)||(position <= comma)){
          ind_buf[position-1] = read_byte_flash(number[tmp]);
       }
       else{
          ind_buf[position-1] = read_byte_flash(number[IND_EMPTY_NUM]);
       }
       value = value/10;
       position++;
       if (position > IND_AMOUNT_NUM){
          break;
       }
    }
    */

	for (int i = start_pos; i < stop_pos + 1; i++) {
		// Buffer[i] = (tmpVal%10 != 0)?(tmpVal%10):(((tmpVal < 10) && (i > 0))?(SYMBOL_BLANK):(tmpVal%10));
		// Buffer[i] = (tmpVal%10 != 0)?(tmpVal%10):(((tmpVal < 10) && (i > 0))?((i < comma_pos)?(0):(SYMBOL_BLANK)):(tmpVal%10));
		Buffer[i] = (tmpVal%10 != 0)?(tmpVal%10):(((tmpVal < 10) && (i > 0))?((i < comma_pos)?(0):(((value == 0) && (i == start_pos))?(0):(SYMBOL_BLANK))):(tmpVal%10));

		tmpVal /= 10;

		if ((i + 1) == comma_pos) SetBit(Buffer[i], 7);
		else ClearBit(Buffer[i], 7);

		if (tmpVal != 0) ZnakPos++;
	}
	if (value < 0) {
		Buffer[(comma_pos == 0)?(ZnakPos + start_pos):(comma_pos)] = SYMBOL_MINUS;
	}
	return ZnakPos;
}
void MAX72xx_SendBuffer(uint8_t ic_addr)
{
	if (ic_addr >= MAX72XX_NUMBERS) return;
	for(int i = 0; i < IND_AMOUNT_NUM; i++) {
		MAX72xx_Send(ic_addr, i + 1, Buffer[(i + ic_addr) + ( ic_addr * (IND_AMOUNT_NUM - 1))]);
	}
}

void MAX72xx_Clear(uint8_t ic_addr)
{
	if (ic_addr >= MAX72XX_NUMBERS) return;
	DecodeMask = 0xFFFFFFFFFFFFFFFF;

	MAX72xx_Send(ic_addr, DECODE_MODE, 0xFF);
	for(int i = 0; i < IND_AMOUNT_NUM; i++) {
		Buffer[(i + ic_addr) + ( ic_addr * (IND_AMOUNT_NUM - 1))] = SYMBOL_BLANK;
	}
	MAX72xx_SendBuffer(ic_addr);
}

void MAX72xx_OutInt(uint8_t ic_addr, int32_t value, uint8_t comma_pos)
{
	uint8_t DigitsNumbers = 0;
	uint8_t TmpMask = 0;

	if ((ic_addr >= MAX72XX_NUMBERS) ||
		(comma_pos > IND_AMOUNT_NUM)) {
		return;
	}

	DigitsNumbers = make_number(value, ic_addr * IND_AMOUNT_NUM, ALL_AMOUNT_NUM - 1, comma_pos + (ic_addr * IND_AMOUNT_NUM));
	for (int j = 0; j < DigitsNumbers; j++) {
		SetBit(DecodeMask, j);
	}
	TmpMask = 0xFF & ((uint8_t)(DecodeMask >> (ic_addr * 8)));
	MAX72xx_Send(ic_addr, DECODE_MODE, TmpMask);
	MAX72xx_SendBuffer(ic_addr);
}

void MAX72xx_OutIntFormat(int32_t value, uint8_t start_pos, uint8_t stop_pos, uint8_t comma_pos)
{
	uint8_t TmpMask = 0;

	if ((start_pos < 1) ||
		(stop_pos > ALL_AMOUNT_NUM) ||
		(comma_pos > ALL_AMOUNT_NUM)) {
		return;
	}

	for (int j = (start_pos - 1); j < stop_pos; j++) {
		SetBit(DecodeMask, j);
	}
	make_number(value, start_pos - 1, stop_pos - 1, comma_pos);
	for (int i = 0; i < MAX72XX_NUMBERS; i++) {
		TmpMask = 0xFF & ((uint8_t)(DecodeMask >> (i * 8)));
		MAX72xx_Send(i, DECODE_MODE, TmpMask);
		MAX72xx_SendBuffer(i);
	}
}

void MAX72xx_SetComma(uint8_t pos, uint8_t val)
{
	if ((pos > ALL_AMOUNT_NUM)) {
		return;
	}

	SetBitVal(Buffer[pos-1], 7, val);
	for (int i = 0; i < MAX72XX_NUMBERS; i++) {
		MAX72xx_SendBuffer(i);
	}
}

void MAX72xx_SetCommaMask(uint8_t ic_addr, uint8_t comma_mask)
{
	if (ic_addr >= MAX72XX_NUMBERS) {
		return;
	}

	for (int i = 0; i < IND_AMOUNT_NUM; i++) {
		SetBitVal(Buffer[ic_addr * 8 + i], 7, (1 & (comma_mask >> i)));
	}
	MAX72xx_SendBuffer(ic_addr);
}

void MAX72xx_SetIntensity(uint8_t ic_addr, uint8_t val)
{
	if (ic_addr >= MAX72XX_NUMBERS) {
		return;
	}
	if ((ic_addr >= MAX72XX_NUMBERS ) || (val > 15)) return;
	MAX72xx_Send(ic_addr, INTENSITY, val);
}
