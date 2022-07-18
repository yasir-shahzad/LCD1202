
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
//Arduino toolchain header, version dependent
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
#include "font.h"

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


#define LCDWIDTH   94   ///< LCD is 94 pixels wide
#define LCDHEIGHT  66   ///< 66 pixels high
  
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
  
  //Instantiate the LCD1202
  LCD1202(u8, u8, u8, u8);
  void initialize();
  void clearScreen();
  void display();
  void writePixel(i8, i8, bool);
  void fillScreen(bool);
  void drawChar(i8, i8, bool, unsigned char);
  void print(i8, i8, bool, char*);
  void print(i8, i8, bool, long);
  void print_1607(i8, i8, bool, char*);
  void drawLine(i8, i8, i8, i8, bool);
  void drawFastVLine(i8, i8, i8, bool);
  void drawFastHLine(i8, i8, i8, bool);
  void drawRect(i8, i8, i8, i8, bool);
  void drawCircle(i8, i8, u16, bool);
  void drawRoundRect(i8, i8, i8, i8, i8, bool);
  void drawTriangle(i8, i8, i8, i8, i8, i8, bool);
  void drawCircleHelper(i8, i8, i8, i8, bool);
  void fillCircle(i8, i8, i8, bool);
  void fillCircleHelper(i8, i8, i8, i8, i8, bool);
  void fillRect(i8, i8, i8, i8, bool);
  void fillRoundRect(i8, i8, i8, i8, i8, bool);
  void fillTriangle(i8, i8, i8, i8, i8, i8, bool);
  void drawBitmap(i8, i8, const char*, i8, i8, bool);
  void simb16x32(i8, i8, bool, i8);
  void simb10x16(i8, i8, bool, i8);
  void customObj(i8, i8, bool, i8);
  void battery(u8,u8,u8, bool);
  void signal (u8,u8,u8);
  void printLcd(char*);
private:
  volatile u8 _rstpin, _cspin, _dinpin, _sckpin;
  
  void sendChar(i8, i8);
};
#endif
