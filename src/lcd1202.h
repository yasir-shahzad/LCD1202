
#ifndef LCD1202_H
#define LCD1202_H
//
// LCD1202
// Library for accessing the STE2007 128x96 LCD display
// Written by Yasir Shahzad (Yasirsahzad918@gmail.com)
// Copyright (c) Yasir Shahazad.
// Project started 6/15/2022
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
#include "Arduino.h"
#include "font.h"

//Arduino toolchain header, version dependent
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

//Alias of Basic Types
#define i32 long int
#define i16 int
#define i8  char
#define u32 unsigned long int
#define u16 unsigned int
#define u8  unsigned char

#define LCD_X        96
#define LCD_Y        68
#define LCD_String    9
  
#define W   94
#define H   66
  
#define SetYAddr   0xB0
#define SetXAddr4  0x00
#define SetXAddr3  0x10

#define LCD_D         1
#define LCD_C         0

i8 LCD_RAM[LCD_X * LCD_String];

#define lcd1202_swap(a, b)                                                     \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation
  
#define pgm     pgm_read_byte

class LCD1202 {
  public:
       unsigned long previousTime;
      
       LCD1202(u8 _rst, u8 _CS, u8 _Data, u8 _Clock);
       void initialize();
       void clearScreen();
       void update();
       void writePixel(i8 x, i8 y, bool color);
       void fillScreen(bool color);
       void drawChar(i8 x, i8 y, bool color, unsigned char c);
       void print(i8 x, i8 y, bool color, char *str);
	   void print(i8 x, i8 y, bool color, long num);
	   void print_1607(i8 x, i8 y, bool color, char *str);
       void drawLine(i8 x0, i8 y0, i8 x1, i8 y1, bool color);
       void drawFastVLine(i8 x, i8 y, i8 h, bool color);
       void drawFastHLine(i8 x, i8 y, i8 w, bool color);
       void drawRect(i8 x, i8 y, i8 w, i8 h, bool color);
       void drawCircle(i8 x0, i8 y0, u16 r, bool color);
       void drawRoundRect(i8 x, i8 y, i8 w, i8 h, i8 r, bool color);
       void drawTriangle(i8 x0, i8 y0, i8 x1, i8 y1, i8 x2, i8 y2, bool color);
       void drawCircleHelper(i8 x0, i8 y0, i8 r, i8 cornername, bool color);
       void fillCircle(i8 x0, i8 y0, i8 r, bool color);
       void fillCircleHelper(i8 x0, i8 y0, i8 r, i8 cornername, i8 delta, bool color);
       void fillRect(i8 x, i8 y, i8 w, i8 h, bool color);
       void fillRoundRect(i8 x, i8 y, i8 w, i8 h, i8 r, bool color);
       void fillTriangle(i8 x0, i8 y0, i8 x1, i8 y1, i8 x2, i8 y2, bool color);
       void drawBitmap(i8 x, i8 y, const char *bitmap, i8 w, i8 h, bool color);
       void simb16x32(i8 x, i8 y, bool color, i8 c);
       void simb10x16(i8 x, i8 y, bool color, i8 c);
       void customObj(i8 x, i8 y, bool color, i8 c);
       void battery(u8,u8,u8, bool);
       void signal (u8,u8,u8);
       void tick (u8,u8);
       void printLcd(char* message);
  private:
       volatile u8 rst_pin, cs_pin, data_pin, clk_pin;
       
       void sendChar(i8 mode, i8 c);
};
#endif
