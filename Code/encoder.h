#ifndef	encoder_h
#define	encoder_h
#include <avr/io.h>

//_________________________________________
//порт и выводы к которым подключен энкодер
#define PORT_Enc 	PORTB
#define PIN_Enc 	PINB
#define DDR_Enc 	DDRB
#define Pin1_Enc 	1
#define Pin2_Enc 	0
//______________________
#define RIGHT_SPIN 0x01 
#define LEFT_SPIN 0xFF

void ENC_InitEncoder(void);
void ENC_PollEncoder(void);
unsigned char ENC_GetStateEncoder(void);
#endif  //encoder_h
