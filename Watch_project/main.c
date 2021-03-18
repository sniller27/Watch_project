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
volatile char data = 0;
volatile char flag_r = 0;

// receive complete interrupt service routine (UART receive interrupt)
// ISR(USART0_RX_vect){
// 	data = UDR0;
// 	flag_r=1;
// }

int main(void)
{  
	
  
//   I2C_Init();  //initialize i2c interface to display
//   InitializeDisplay(); //initialize  display
//   
//    print_fonts();  //for test and then exclude the  clear_display(); call
//    //char text[]="en tekst string"; //string declared before use it in sendStrXY - 15 chars long incl spaces
//    clear_display();   //use this before writing you own text
   
   uart0_init(MYUBRRF); // UART0 init
   
   //initRXCIntr(); // init interrupt RX interrupt (receive interrupt)
   
   //sei(); // enable global interrupt (prevents putchUSART0(getchUSART0()); from working)
   
   
   //char var[4];
   int i = 0;
   char buffer[20] = {0};
   
  while (1)
  {  
	 
	 //sprintf(var,"%c",getchUSART0());
	 //sendStrXY(var,0,0);
	 
	 // OLD
	 putchUSART0(getchUSART0());
	 //_delay_ms(1000);
	 
	 
	 // NEW
	// i = getsUSART0(buffer, 8);
	 //if(i==8)
	 //putsUSART0(buffer); 
	 //retransmit buffer
	 //hh:mm:ss
		  
	 // NEW NEW
// 	 if (flag_r==1)
// 	 {
// 		flag_r = 0;
// 		putchUSART0(data); // transmit
// 	 }
	 

	 
	
	 // med string	 
	 // i main
	 // lav string
	 // char tidstekst[]="timer:min:sek \0";
	 // putsUSART0(tidstekst);

	 
	 
	 //var = getchUSART0();
	 
	 
	 //sendCharXY(var,1,2);  //one char  - X is line number - from 0 -7 and Y number position of the char an the line - 15 chars 
	 
	 //sendStrXY(text,0,0); //line 0  -print the line of text
	  
  }
  

}