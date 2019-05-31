/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
*/

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/sfr_defs.h>
#include <avr/pgmspace.h>
#include "nrf24.h"


/* ------------------------------------------------------------------------- */
/* Software UART routine. */
/* 9600 bps. Transmit only */
/* Transmit pin is: B2 */
/* ------------------------------------------------------------------------- */
/* Hardware specific section ... */
/* ------------------------------------------------------------------------- */
#define F_CPU 8000000UL
#include <util/delay.h>
#include "USART_RS232_H_file.h"
/*--------------------------------------------------------------------------*/
uint8_t temp;
uint8_t data_array[2] ;
uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};

/* ------------------------------------------------------------------------- */
void check_data();
uint16_t read_ADC(uint8_t AD);
void binary_print (uint8_t x);

int main()
{
	DDRC |= (1 << 3);
	PORTC |= (1 << 3); 
	// khai bao ADC
	ADMUX |= (1 << REFS0);  
	ADCSRA |= (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0);
    /* init the software uart */
    USART_Init(9600);
    /* init hardware pins */
    nrf24_init();
    
    /* Channel #2 , payload length: 10 */
    nrf24_config(2,2);

    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);    
	_delay_ms(100);
	data_array[0] = 8;
    while(1)
    {                                                
        /* Automatically goes to TX mode */
		
		check_data();		// check data on the keyboard , prepare the mode off Robot
        nrf24_send(data_array);		// after prepare the mode ==> send the string off mode goto reciver module 
        
        /* Wait for transmission to end */
		
        while(nrf24_isSending());	//wait for the data to be send
		
        /* Make analysis on last tranmission attempt */
		nrf24_lastMessageStatus();
		//binary_print(data_array[0]);
		if (nrf24_lastMessageStatus() == NRF24_TRANSMISSON_OK ) { PORTC &= ~(1 << 3);}
		else {
			PORTC |= (1 << 3);
		}
		binary_print(data_array[0]);
	}
	return 0;
}

void check_data()
{
	data_array[0] = 0;	//khoi tao gia tri ban dau cua data
	uint16_t speed = read_ADC(0);	//read speed 
	uint16_t Vx = read_ADC(1);		// read Vx on joystick module
	uint16_t Vy = read_ADC(2);		// read Vy on joysitch module 
	if (Vx > 900) data_array[0] |= (1 << 7);	// tien
	if (Vx < 250) data_array[0] |= (1 << 6);	// lui
	if (Vy > 900) data_array[0] |= (1 << 5);	// phai
	if (Vy < 250) data_array[0] |= (1 << 4);	// trai
	/*			chose the speed off motor by using bien_tro			*/
	if (speed <= 102.3) data_array[0] |= 1;
	else
	if (speed <= 102.3*2) data_array[0] |= 2;
	else
	if (speed <= 102.3*3) data_array[0] |= 3;
	else
	if (speed <= 102.3*4) data_array[0] |= 4;
	else
	if (speed <= 102.3*5) data_array[0] |= 5;
	else
	if (speed <= 102.3*6) data_array[0] |= 6;
	else
	if (speed <= 102.3*7) data_array[0] |= 7;
	else
	if (speed <= 102.3*8) data_array[0] |= 8;
	else
	if (speed <= 102.3*9) data_array[0] |= 9;
	else
	if (speed <= 102.3*10) data_array[0] |= 10;
	
	
	return;
}

uint16_t read_ADC(uint8_t AD) // check the analog sign in AD PIN
{
	ADMUX = (1<< REFS0)| AD;
	ADCSRA |= (1 << ADSC);
	while (bit_is_clear(ADCSRA, ADIF));
	return ADCW;
}

void binary_print (uint8_t x) {
	for (int i=7; i>=0 ; i--) {
		if(bit_is_set(x,i)) {
			USART_TxChar('1');
		}
		else USART_TxChar('0');
	}
	USART_TxChar('\n');
	return;
}