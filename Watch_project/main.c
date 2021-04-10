/*
 * opg1_ny_E20.c
 *
 * Created: 01-09-2020 14:34:13
 * Author : osch
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "I2C.h"  //include library for i2c driver
#include "ssd1306.h" //include display driver
#include <avr/interrupt.h>

#define F_CPU 16000000UL //UL = unsigned long
#include <util/delay.h>
#include "UART/UART_MODULE.h"

// For USART
#define BAUD 19200 // could also be written as 103 (according to table s.231 i datablad)
#define MYUBRRF F_CPU/8/BAUD-1 // full duplex
#define MYUBRRH F_CPU/16/BAUD-1 // half duplex

// Variables for UART receive interrupt
#define MAX 11
char buffer[MAX] = {0};
char initial_time[MAX] = {0};
volatile char data = 0;
volatile char flag_ub = 0;

int timer1_counter = 0;
int timer1_flag = 0;

char message[] = "Write current time in hh:mm:ss\n";

// reset button
volatile unsigned char flag;

// receive complete interrupt service routine (UART receive interrupt)
ISR(USART0_RX_vect){
	// some existing stuff
	static int i=0;
	buffer[i++]=UDR0;
	
	// some existing stuff
	if(i==MAX-1){
		flag_ub=1;
		i=0; 
	}
	
}

void enableReceive_Itr(){
	UCSR0B|=(1<<RXCIE0); // enable receive complete interrupt
}

// for reset button (eksternt interrupt)
ISR(INT4_vect){
	
	_delay_ms(30);
	
	// empties buffer (empties array)
	memset(buffer, 0, sizeof MAX);
	
	// send user-input-message
	putsUSART0(message);
	
	_delay_ms(30);

}

void init_timer1(){
	TCCR1B |=(1<<WGM12); // timer mode: CTC
	TCCR1B |=(1<<CS11)|((1<<CS10)); // timer pre-scaling: 64
	OCR1A = 249; // udregnet (sætter værdi i sammenligningsregister)
	TIMSK1 |=(1<<OCIE1A); // interrupt mode: Output Compare A Match Interrupt Enable
}
// compare match interrupt service routine til timer
ISR(TIMER1_COMPA_vect)
{
	timer1_counter++;
	if (timer1_counter == 999)
	{
		timer1_flag = 1;
		timer1_counter = 0;
	}
}

int main(void)
{  

  _delay_ms(3000); // to prevent "trash-text" when connecting
     
   uart0_init(MYUBRRF); // UART0 init
   init_timer1(); // init timer1 (alternativ måde at lave delays)
   
   putsUSART0(message); // terminal startup message
   
   enableReceive_Itr(); // init interrupt RX interrupt (receive interrupt)
   sei(); // enable global interrupt (prevents putchUSART0(getchUSART0()); from working)
   
   // FOR BLINKING LED
   // configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560) (alternatively: DDRB = 0b10000000;)
   DDRB |= (1<<PB7); //DDRB = 0x80; DDRB = 0b10000000;
   
   // PUSH BUTTON
   // Set PINE4 as input
   DDRE&=~(1<<PE4); //DDRE = 0b00000000;
   // Set PINE4 (D2) high
   PINE |= (1<<PE4);  // PINE = 0b00010000;
   // interupts
   EICRB|=(1<<ISC41); // sætter bits i control register med interrupt sense control (bit 1)
   EICRB&=~(1<<ISC40); // sætter bits i control register med interrupt sense control (bit 2)
   EIMSK|=(1<<INT4); // set interrupt bit mask for int4 (for push button)
	
   // timer variables
   char str_ss[2];
   int sec = 0;

   char str_m[2];
   int min = 0;

   char str_h[2];
   int hour = 0;

   // interrupt flag (for reset button)
   flag = 0;
   
  while (1)
  {  
	 
	 // OLD
	 //putchUSART0(getchUSART0());
	 //_delay_ms(1000);
	 
	 // NEW
	 //i = getsUSART0(buffer, 8);
	 //if(i==8)
	 //putsUSART0(buffer);
	 //retransmit buffer
	 //hh:mm:ss
		  
	 // NEW NEW
	 if (flag_ub==1)
	 {
		flag_ub = 0;
		putsUSART0(buffer);//return the buffer (string sent to terminal)
	 }
	 
	 // wait 1s
 	 //_delay_ms(1000);
 	 if (timer1_flag == 1)
 	 {
		// GET DATA FROM BUFFER (as ascii)
		//sec = atoi(str_ss); // ascii to int
		// (buffer[7]-0x30) konverter fra char til int (via ascii-table, specifikt med hex)
		sec = ((buffer[7]-0x30)+((buffer[6]-0x30)*10));
		min = ((buffer[4]-0x30)+((buffer[3]-0x30)*10));
		hour = ((buffer[1]-0x30)+((buffer[0]-0x30)*10));
		
		// MANIPULATE BUFFER-DATA (as integers)
		sec++;
		
		if (sec == 60)
		{
			min++;
			sec = 0;
			
			if (min == 60)
			{
				hour++;
				min = 0;
				
				sprintf(str_h, "%i", hour);
				
				if (hour == 24)
				{
					//reset
					buffer[0] = '0';
					buffer[1] = '0';
					}else if (hour < 10) {
					buffer[0] = '0';
					buffer[1] = str_h[0];
					}else {
					buffer[0] = str_h[0];
					buffer[1] = str_h[1];
				}
			}
			
			sprintf(str_m, "%i", min);
			if (min < 10)
			{
				buffer[3] = '0';
				buffer[4] = str_m[0];
				}else {
				buffer[3] = str_m[0];
				buffer[4] = str_m[1];
			}
			
		}

		//sprintf(ary, "%d:%d:%d",hour,min,sec);
		
		// WRITE DATA BACK TO BUFFER (as ascii)
		sprintf(str_ss, "%i", sec);
		
		if (sec < 10)
		{
			buffer[6] = '0';
			buffer[7] = str_ss[0];
			}else {
			buffer[6] = str_ss[0];
			buffer[7] = str_ss[1];
		}
		
		putsUSART0(buffer);
		
		timer1_flag = 0;
 	 }
  
  }
  
}