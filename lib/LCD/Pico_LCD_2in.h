/*****************************************************************************
* | File      	:	Pico_LCD_2in.h
* | Author      :   Luckfox team
* | Function    :   LCD driver
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2023-09-03
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef __PICO_LCD_2IN__H
#define __PICO_LCD_2IN__H

#include "DEV_Config.h"

#define PICO_LCD_2IN_WIDTH   240 //LCD width
#define PICO_LCD_2IN_HEIGHT  320 //LCD height


#define PICO_LCD_2IN_CS_0	LCD_CS_0	 
#define PICO_LCD_2IN_CS_1	LCD_CS_1	
	                  
#define PICO_LCD_2IN_RST_0	LCD_RST_0	
#define PICO_LCD_2IN_RST_1	LCD_RST_1	
	                  
#define PICO_LCD_2IN_DC_0	LCD_DC_0	
#define PICO_LCD_2IN_DC_1	LCD_DC_1	
	                  
#define PICO_LCD_2IN_BL_0	LCD_BL_0	
#define PICO_LCD_2IN_BL_1	LCD_BL_1	

void PICO_LCD_2IN_Init(void); 
void PICO_LCD_2IN_Clear(UWORD Color);
void PICO_LCD_2IN_Display(UBYTE *image);
void PICO_LCD_2IN_DrawPaint(UWORD x, UWORD y, UWORD Color);
void  Handler_2IN_LCD(int signo);

void PICO_LCD_2IN_WriteData_Word(UWORD da);
void PICO_LCD_2IN_SetCursor(UWORD X, UWORD Y);
void PICO_LCD_2IN_SetWindow(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD  Yend);
void PICO_LCD_2IN_ClearWindow(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD color);


#endif
