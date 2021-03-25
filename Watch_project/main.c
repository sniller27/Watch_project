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
volatile char data = 0;
volatile char flag_ub = 0;

// receive complete interrupt service routine (UART receive interrupt)
ISR(USART0_RX_vect){
	static int i=0;
	buffer[i++]=UDR0;
	
	if(i==MAX-1){
		flag_ub=1;
		i=0; 
	}
	
}

void enableReceive_Itr(){
	UCSR0B|=(1<<RXCIE0); // enable receive complete interrupt
}

int main(void)
{  
	
  _delay_ms(3000); // to prevent "trash-text" when connecting
  
//   I2C_Init();  //initialize i2c interface to display
//   InitializeDisplay(); //initialize  display
//   
//    print_fonts();  //for test and then exclude the  clear_display(); call
//    //char text[]="en tekst string"; //string declared before use it in sendStrXY - 15 chars long incl spaces
//    clear_display();   //use this before writing you own text
   
   uart0_init(MYUBRRF); // UART0 init
   
   //char message[] = "Please, enter time in the following format : \0";
   //char message[] = "Write current time in hh:mm:ss \0 \n";
   //putsUSART0(message);
   //char message[] = "Please, enter time in the following format : \0";
   char message[] = "Write current time in hh:mm:ss\n";
   putsUSART0(message);
   
   enableReceive_Itr(); // init interrupt RX interrupt (receive interrupt)
   sei(); // enable global interrupt (prevents putchUSART0(getchUSART0()); from working)
   
   //char var[4];
   //int i = 0;
	
   // timer variables
   char str_ss[2];
   int sec = 0;

   char str_m[2];
   int min = 0;

   char str_h[2];
   int hour = 0;

   
  while (1)
  {  
	 
	 //sprintf(var,"%c",getchUSART0());
	 //sendStrXY(var,0,0);
	 
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
 	 _delay_ms(1000);
	  
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

			 if (hour < 10)
			 {
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
		 
		 if (hour == 24)
		 {
			 buffer[0] = '0';
			 buffer[1] = '0';
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
	 
	 //var = getchUSART0();
	 
	 //sendCharXY(var,1,2);  //one char  - X is line number - from 0 -7 and Y number position of the char an the line - 15 chars 
	 
	 //sendStrXY(text,0,0); //line 0  -print the line of text
	  
  }
  

}