/**
 * ssd1306.c
 *
 * Created: 03-01-2018 17:33:26
 *  Author: osc
 *this modules purpose is to initiate and control the OLED display SSD1306 driver for the 128x64 OLED display
 *The plus can be connected to PIN 24 and the GND to PIN 26 and sda goes to PIN 21 and the sck goes to PIN 20
 *http://microcontrolandos.blogspot.dk/2014/12/pic-ssd1306.html
 * SSD1306 Advanced information Matrix apr. 2008  rev. 1.1 www.solomon-systech.com describes all used commands and data used for the display 128x64 dot matrix
 *for Mega2560 arduino board
 *p reference to this documentation
 *initialization is from the DD-2864bY-3A rev c p 19
 *both data sheets are at git hub in this project
 */
#include <math.h>
#include <string.h>
#include "I2C.h"
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "ssd1306.h"
#include "data.h"
#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }
#define _vccstate 1  //externalVcc

uint8_t _i2c_address=0x78;    //display write address

/**write a command to the ssd1306*/
void  ssd1306_command(uint8_t c)
{
	uint8_t control = 0x00; // some use 0X00 other examples use 0X80. I tried both
	I2C_Start(_i2c_address);
	//I2C_Write();
	I2C_Write(control); // This is Command
	I2C_Write(c);
	I2C_Stop();
}
////////////////////////////////////////////
//
/**write a a data byte to the ssd1306*/
void  ssd1306_data(uint8_t c)
{
	I2C_Start(_i2c_address);
	I2C_Write(_i2c_address);
	I2C_Write(0X40); // This byte is DATA
	I2C_Write(c);
	I2C_Stop();
}
///////////////////////////////////////////////////
/** Used when doing Horizontal or Vertical Addressing*/
void setColAddress()
{
	ssd1306_command(SSD1306_COLUMNADDR); // 0x21 COMMAND
	ssd1306_command(0); // Column start address
	ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address
}
/////////////////////////////////////////////////////
/** Used when doing Horizontal or Vertical Addressing*/
void setPageAddress()
{
	ssd1306_command(SSD1306_PAGEADDR); // 0x22 COMMAND
	ssd1306_command(0); // Start Page address
	ssd1306_command((SSD1306_LCDHEIGHT/8)-1);// End Page address
}
///////////////////////////////////////////////////////////////////
/** init according to SSD1306 data sheet and using the plus can be connected to PIN 24 and the GND to PIN 26 */

void  InitializeDisplay()
{
	


	// Init sequence for 128x64 OLED module
	ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE

	ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	ssd1306_command(0x80);                 // the suggested ratio 0x80

	ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
	ssd1306_command(0x3F);

	ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	ssd1306_command(0x0);                                   // no offset

	ssd1306_command(SSD1306_SETSTARTLINE);// | 0x0);        // line #0

	ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
	ssd1306_command(0x14);  // using internal VCC

	ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
	ssd1306_command(0x00);                         // 0x00 horizontal addressing automatic line shift

	ssd1306_command(SSD1306_SEGREMAP | 0x1); // rotate screen 180

	ssd1306_command(SSD1306_COMSCANDEC); // rotate screen 180

	ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	ssd1306_command(0x12);

	ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
	ssd1306_command(0xCF);

	ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
	ssd1306_command(0xF1);

	ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
	ssd1306_command(0x40);

	ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4

	ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

	ssd1306_command(SSD1306_DISPLAYON);                     //switch on OLED

}

/** reset the display*/
void reset_display(void)
{
	displayOff();

	clear_display();


	displayOn();
}




//==========================================================//
/** Turns display on.*/
void displayOn(void)
{
	ssd1306_command(0xaf);        //display on p. 28
}

//==========================================================//
/** Turns display off.*/
void displayOff(void)
{
	ssd1306_command(0xae);		//display off p. 28
}

//==========================================================//
/** Clears the display by sending 0 to all the screen map.*/
void clear_display(void)
{
	unsigned char i,k;
	for(k=0;k<8;k++)
	{
		setXY(k,0);
		{
			for(i=0;i<128;i++)     //clear all COL
			{
				SendChar(0);         //clear all COL
				//delay(10);
			}
		}
	}
}






//==========================================================//
/**print integer string in big font*/
void printBigTime(char *string)
{

	int Y=0;
	int lon = strlen(string);
	if(lon == 3) {
		Y = 0;
		} else if (lon == 2) {
		Y = 3;
		} else if (lon == 1) {
		Y = 6;
	}

	int X = 4;
	while(*string)
	{
		printBigNumber(*string, X, Y);

		Y+=3;
		X=4;
		setXY(X,Y);
		string++;
	}
}


//==========================================================//
/** Prints a display big number (96 bytes) in coordinates X Y,
* being multiples of 8. This means we have 16 COLS (0-15)
* and 8 ROWS (0-7).*/
void printBigNumber(char string, int X, int Y)
{
	setXY(X,Y);               //set the cursor to start
	int salto=0;
	for(int i=0;i<96;i++)
	{
		if(string == ' ') {
			SendChar(0);
		} else
		SendChar(pgm_read_byte(bigNumbers[string-0x30]+i));

		if(salto == 23) {
			salto = 0;
			X++;
			setXY(X,Y);
			} else {
			salto++;
		}
	}
}

//==========================================================//
/** Actually this sends a byte, not a char to draw in the display.
* Displays chars uses 8 byte font the small ones and 96 bytes
* for the big number font.*/
void SendChar(unsigned char data)
{
	I2C_Start(_i2c_address); // begin transmitting
	I2C_Write(0x40);//data mode
	I2C_Write(data);
	I2C_Stop();    // stop transmitting
}

//==========================================================//
/** Prints a display char (not just a byte) in coordinates X Y,
* being multiples of 8. This means we have 16 COLS (0-15)
* and 8 ROWS (0-7).*/
void sendCharXY(unsigned char data, int X, int Y)
{
	setXY(X, Y);
	I2C_Start(_i2c_address); // begin transmitting
	I2C_Write(0x40);//data mode

	for(int i=0;i<8;i++)
	I2C_Write(pgm_read_byte(myFont[data-0x20]+i));

	I2C_Stop();    // stop transmitting
}

//==========================================================//
/** Set the cursor position in a 16 COL * 8 ROW map.*/
void setXY(unsigned char row,unsigned char col)
{
	ssd1306_command(0xb0+row);                //set page address    p. 31
	ssd1306_command(0x00+(8*col&0x0f));       //set low col address   p. 30
	ssd1306_command(0x10+((8*col>>4)&0x0f));  //set high col address   p.30
}


//==========================================================//
/** Prints a string regardless the cursor position.*/
void sendStr(char *string)
{
	unsigned char i=0;
	while(*string)
	{
		for(i=0;i<8;i++)
		{
			SendChar(pgm_read_byte(myFont[*string-0x20]+i));   //look up ascii chars (no danish) defined in data.h
		}
		string++;
	}
}

//==========================================================//
/** Prints a string in coordinates X Y, being multiples of 8.
* This means we have 16 COLS (0-15) and 8 ROWS (0-7).*/
void sendStrXY( char *string, int X, int Y)
{
	setXY(X,Y);
	unsigned char i=0;
	while(*string)
	{    if (*string=='\n'){
		setXY(X+1,0);
		string++;
		}
		for(i=0;i<8;i++)
		{
			SendChar(pgm_read_byte(myFont[*string-0x20]+i));
		}
		string++;
	}
}
void ssd1306_setpos(uint8_t x, uint8_t y)
{

	ssd1306_command(0xb0 + y);
	ssd1306_command(((x & 0xf0) >> 4) | 0x10); // | 0x10


}
void print_fonts(){
	clear_display();

	uint8_t data=32;
	for(int k=0;k<6;k++){
		setXY(k,0);

		for (int j=0;j<16;j++)
		{

			for(int i=0;i<8;i++){
			SendChar(pgm_read_byte(myFont[(data+j)-0x20]+i));
			}
		}
		data=data+16;
	}
	}
void ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[])
{
	uint16_t j = 0;
	uint8_t y;
	if (y1 % 8 == 0) y = y1 / 8;
	else y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		ssd1306_setpos(x0,y);

		for (uint8_t x = x0; x < x1; x++)
		{
			 ssd1306_data(pgm_read_byte(&bitmap[j++]));
		}

	}
}
/*
void drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
	return;

	// check rotation, move pixel around if necessary
	switch (getRotation()) {
		case 1:
		ssd1306_swap(x, y);
		x = WIDTH - x - 1;
		break;
		case 2:
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		break;
		case 3:
		ssd1306_swap(x, y);
		y = HEIGHT - y - 1;
		break;
	}

	// x is which column
	switch (color)
	{
		case WHITE:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] |=  (1 << (y&7)); break;
		case BLACK:   buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~(1 << (y&7)); break;
		case INVERSE: buffer[x+ (y/8)*SSD1306_LCDWIDTH] ^=  (1 << (y&7)); break;
	}

}
*/
void invertDisplay(uint8_t i) {
	if (i) {
		ssd1306_command(SSD1306_INVERTDISPLAY);
		} else {
		ssd1306_command(SSD1306_NORMALDISPLAY);
	}
}
/** startscrollright
* Activate a right handed scroll for rows start through stop
* Hint, the display is 16 rows tall. To scroll the whole display, run:
* scroll right(0x00, 0x0F)*/
void startscrollright(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X00);
	ssd1306_command(0XFF);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}
/** startscrollleft
* Activate a right handed scroll for rows start through stop
*Hint, the display is 16 rows tall. To scroll the whole display, run:
* scrollleft(0x00, 0x0F) */
void startscrollleft(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X00);
	ssd1306_command(0XFF);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}
/** startscrolldiagright
*Activate a diagonal scroll for rows start through stop
* Hint, the display is 16 rows tall. To scroll the whole display, run:
* display.scrollright(0x00, 0x0F)*/
void startscrolldiagright(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	ssd1306_command(0X00);
	ssd1306_command(SSD1306_LCDHEIGHT);
	ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}
/** startscrolldiagleft
* Activate a diagonal scroll for rows start through stop
*Hint, the display is 16 rows tall. To scroll the whole display, run:
* display.scrollright(0x00, 0x0F)*/
void startscrolldiagleft(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	ssd1306_command(0X00);
	ssd1306_command(SSD1306_LCDHEIGHT);
	ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}
void stopscroll(void){
	ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void dim(bool dim) {
	uint8_t contrast;

	if (dim) {
		contrast = 0; // Dimmed display
		} else {
		if (_vccstate == SSD1306_EXTERNALVCC) {
			contrast = 0x9F;
			} else {
			contrast = 0xCF;
		}
	}
	// the range of contrast to too small to be really useful
	// it is useful to dim the display
	ssd1306_command(SSD1306_SETCONTRAST);
	ssd1306_command(contrast);
}