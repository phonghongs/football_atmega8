#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "my_library.h"

#define true 1
#define false 0

#define BTN3	2
#define BTN5	3
#define BTN2	4
#define BTN4	5
#define BTN0	6
#define BTN1	7

//===================BUTTON + SWITCH=====================
uint8_t get_button(uint8_t keyid)
{
	if(keyid==BTN0 || keyid==BTN1) {
		if(keyid==BTN0) {
			if(bit_is_clear(PINA,3)) {
				return true;
			}
			else return false;
		}
		if(keyid==BTN1) {
			if(bit_is_clear(PINA,4)) {
				return true;
			}
			else return false;
		}
	}
	else {
		if(bit_is_clear(PIND,keyid)) {
			return true;
		}
		else return false;
	}
}

uint16_t adc_read(uint8_t ch)
{
	ADMUX = (1<< REFS0)|ch;									// selecting channel
	ADCSRA|=(1<<ADSC);										// start conversion
	while(!(ADCSRA & (1<<ADIF)));							// waiting for ADIF, conversion complete
	return ADCW;											// Giá trị trả về từ [0 -> 1024] tương ứng [0V -> 5V]	
}

//=======================INITIAL=========================
void INIT()
{
	/*==================button===================*/
	DDRD=(0x00);
	PORTD|=((1<<BTN2)|(1<<BTN3)|(1<<BTN4)|(1<<BTN5)); //tro keo len
	cbi(PORTA,3);
	cbi(PORTA,4);
	PORTA|=(1<<3)|(1<<4);
	//=======================USART================================
	DDRD|=(1<<0)|(1<<1);
	//===============================ADC========================================
	ADMUX=(1<<REFS0);										// 0b0100000000 Chọn điện áp tham chiếu từ chân AVCC, thêm tụ ở AREF
	ADCSRA=(1<<ADEN) | (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	// 0b10000111 Enable ADC and set Prescaler = 128
}