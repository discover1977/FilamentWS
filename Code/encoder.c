#include "encoder.h"
#include "bits_macros.h"

//это дл€ нагл€дности кода
#define b00000011 3
#define b11010010 210
#define b11100001 225

volatile unsigned char bufEnc = 0; //буфер энкодера

//функци€ инициализации
//__________________________________________
void ENC_InitEncoder(void)
{
  ClearBit(DDR_Enc, Pin1_Enc); //вход
  ClearBit(DDR_Enc, Pin2_Enc);
  SetBit(PORT_Enc, Pin1_Enc);//вкл подт€гивающий резистор
  SetBit(PORT_Enc, Pin2_Enc);
}

//функци€ опроса энкодера
//___________________________________________
void ENC_PollEncoder(void)
{
	static unsigned char stateEnc; 	//хранит последовательность состо€ний энкодера
	unsigned char tmp;  
	unsigned char currentState = 0;

	//провер€ем состо€ние выводов микроконтроллера
	if ((PIN_Enc & (1<<Pin1_Enc))!= 0) {SetBit(currentState,0);}
	if ((PIN_Enc & (1<<Pin2_Enc))!= 0) {SetBit(currentState,1);}
		
	// опрос клавиатуры

	//если равно предыдущему, то выходим
	tmp = stateEnc;
	if (currentState == (tmp & b00000011)) return;

	//если не равно, то сдвигаем и сохран€ем в озу
	tmp = (tmp<<2)|currentState;
	stateEnc = tmp;

	//сравниваем получившуюс€ последовательность
	if (tmp == b11100001) bufEnc = LEFT_SPIN;
	if (tmp == b11010010) bufEnc = RIGHT_SPIN;
		
	return;
}

//функци€ возвращающа€ значение буфера энкодера
//_____________________________________________
unsigned char ENC_GetStateEncoder(void)
{
  unsigned char tmp = bufEnc;
  bufEnc = 0;
  return tmp;
}
