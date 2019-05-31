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
#include "nrf24.h"
#include "my_library.h"
#include <stdlib.h>
#include <avr/wdt.h>
/*--------------------------------------------------*/
#define DDR_MOTOR_L DDRB
#define DDR_MOTOR_R DDRB

#define PORT_MOTOR_L PORTB
#define PORT_MOTOR_R PORTB
#define MOTOR_L 2
#define MOTOR_R 3

#define DDR_DIR0 DDRC
#define PORT_DIR0 PORTC
#define DDR_DIR1 DDRC
#define PORT_DIR1 PORTC
#define DIR00	0
#define DIR01	1
#define DIR10	2
#define DIR11   4

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
/* ------------------------------------------------------------------------- */
uint8_t temp;
uint8_t data_array[2];
uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
// define funtion
void binary_print (uint8_t x);
void Timer1_Init(void);
void prepare(void);

float percent = 0.0;
/* ------------------------------------------------------------------------- */
int main()
{
	//wdt_enable( WDTO_2S);
	/* define UART */
	USART_Init(9600);
	/*	define PinMode	*/
	Timer1_Init();
	
    /* init hardware pins */
    nrf24_init();
	   
    /* Channel #2 , payload length: 4 */
    nrf24_config(2,2);
 
    /* Set the device addresses */
    nrf24_tx_address(tx_address);
    nrf24_rx_address(rx_address);
	
    while(1)
    {    
	
      if(nrf24_dataReady())
        {
			nrf24_getData(data_array);
			binary_print(data_array[0]);
			prepare();
			binary_print(data_array[0]);
        }
	  

    }
}
/* ------------------------------------------------------------------------- */
void binary_print (uint8_t x) {
	for (int i=7; i>=0 ; i--) {
		if(bit_is_set(x,i)) {
			USART_TxChar('1');
		}
		else USART_TxChar('0');
	}
	USART_TxChar('\n');
}

void Timer1_Init(void) {
	DDRB |= (1 << 3);
	DDR_MOTOR_L |= (1<<MOTOR_L);
	DDRB |= (1 << 1);
	
	sbi(DDR_DIR0, DIR00);
	sbi(PORT_DIR0,DIR00);
	
	sbi(DDR_DIR0, DIR01);
	cbi(PORT_DIR0,DIR01);
	
	sbi(DDR_DIR1, DIR10);
	sbi(PORT_DIR1, DIR10);
	
	sbi(DDR_DIR1, DIR11);
	cbi(PORT_DIR1, DIR11);
	
	TCCR1A =(1 << COM1A1)|(1<<COM1B1);
	TCCR1A |= (1<<WGM11);
	TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS11);
	//TCCR2 = (1 << WGM21) | (1 << WGM20) |(1 << COM21) | (1 << CS21);
	ICR1=500;
}

void speed2(float left, float right){
	//==========================left==============
	if (left < 0) { //-------------------di lui---------
		sbi(PORT_DIR1,DIR00);
		cbi(PORT_DIR1,DIR01);
	} else{         //----------------tien---------------
		cbi(PORT_DIR1,DIR00);
		sbi(PORT_DIR1,DIR01);
	}
	//===================right====================
	if (right < 0) { //-------------------di lui---------
		sbi(PORT_DIR0,DIR10);
		cbi(PORT_DIR0,DIR11);
		} else{         //----------------tien---------------
		cbi(PORT_DIR0,DIR10);
		sbi(PORT_DIR0,DIR11);
	}
	int speed_right = (long)((abs(right)*percent*1.02)/100.0);
	int speed_left = (long)((abs(left)*percent)/100.0);
	if (speed_right > 500) OCR1A = 500;
	else OCR1A = speed_right;
	if (speed_left > 500) OCR1B = 500;
	else OCR1B = speed_left;
	return;
}

void prepare(void)
{
	/*		speed Mode :	----------	
				+	0001: 1
				+	0010: 2
				+	0011: 3
				+	0100: 4
				+	0101: 5
				+	0110: 6
				+	0111: 7
				+	1000: 8
				+	1001: 9
				+	1010: 10
	----------------------------------*/
	
	if (bit_is_set(data_array[0],3) && bit_is_set(data_array[0], 1)) {percent = 100.0; binary_print(10);}
	else
		if (bit_is_set(data_array[0], 3) && bit_is_set(data_array[0], 0)) {percent = 90.0; binary_print(9);}
	else
		if (bit_is_set(data_array[0], 3))  {percent = 80.0; binary_print(8);}
	else
		if (bit_is_set(data_array[0], 2) && bit_is_set(data_array[0], 1) && bit_is_set(data_array[0], 0))  {percent = 70.0; binary_print(7);}
	else
		if (bit_is_set(data_array[0], 2) && bit_is_set(data_array[0], 1))  {percent = 60.0; binary_print(6);}
	else
		if (bit_is_set(data_array[0],2) && bit_is_set(data_array[0], 0))  {percent = 50.0; binary_print(5);}
	else
		if (bit_is_set(data_array[0],2)) {percent = 40.0; binary_print(4);}
	else
		if (bit_is_set(data_array[0],1) && bit_is_set(data_array[0], 0))  {percent = 30.0; binary_print(3);}
	else
		if (bit_is_set(data_array[0],1))  {percent = 20.0; binary_print(2);}
	else
		if (bit_is_set(data_array[0], 0))  {percent = 10.0; binary_print(1);}
						
					
	//-------------------------		Angle prepare	---------------
	if (bit_is_set(data_array[0], 7) && bit_is_set(data_array[0], 5)) speed2(500.0,300.0);// tien phai'
	else
		if (bit_is_set(data_array[0], 7) && bit_is_set(data_array[0], 4)) speed2(250.0,500.0);// tien trai'
	else
		if (bit_is_set(data_array[0], 6) && bit_is_set(data_array[0], 5)) speed2(-500.0,-350.0);// lui phai'
	else
		if (bit_is_set(data_array[0], 6) && bit_is_set(data_array[0], 4)) speed2(-350.0,-500.0);// lui trai'
	else
		if (bit_is_set(data_array[0], 7)) speed2(500.0,500.0);// tien
	else 
		if (bit_is_set(data_array[0], 6)) speed2(-500.0,-500.0);// lui
	else
		if (bit_is_set(data_array[0],5)) speed2(300.0,-300.0);//	phai
	else
		if (bit_is_set(data_array[0],4)) speed2(-300.0,300.0);// trai
	else speed2(0.0,0.0);//stop
	
	
}