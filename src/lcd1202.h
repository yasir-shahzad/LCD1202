
#ifndef LCD1202_H
#define LCD1202_H
//
// LCD1202
// Library for accessing the STE2007 96x68 LCD display
// Written by Yasir Shahzad (Yasirsahzad918@gmail.com)
// Copyright (c) Yasir Shahazad.
// Project started 6/15/2022
//
// Licensed under the GPL License, Version 2.0 (the "License");
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

#define LCDWIDTH      96  ///< LCD is 96 pixels wide
#define LCDHEIGHT     68  ///< 68 pixels high
#define LCD_String    9
  
#define SetYAddr   0xB0
#define SetXAddr4  0x00
#define SetXAddr3  0x10

#define LCD_D         1
#define LCD_C         0

char LCD_RAM[LCDWIDTH * LCD_String];

#define lcd1202_swap(a, b)                                                     \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation
  
#define pgm     pgm_read_byte

class LCD1202 {
public:
  unsigned long previousTime;
  
  //Instantiate the LCD1202
  LCD1202(uint8_t, uint8_t, uint8_t, uint8_t);
  void initialize();
  void clearScreen();
  void display();
  void writePixel(char, char, bool);
  void fillScreen(bool);
  void drawChar(char, char, bool, uint8_t);
  void print(char, char, bool, char*);
  void print(char, char, bool, int32_t);
  void print_1607(char, char, bool, char*);
  void drawLine(char, char, char, char, bool);
  void drawFastVLine(char, char, char, bool);
  void drawFastHLine(char, char, char, bool);
  void drawRect(char, char, char, char, bool);
  void drawCircle(char, char, uint16_t, bool);
  void drawRoundRect(char, char, char, char, char, bool);
  void drawTriangle(char, char, char, char, char, char, bool);
  void drawCircleHelper(char, char, char, char, bool);
  void fillCircle(char, char, char, bool);
  void fillCircleHelper(char, char, char, char, char, bool);
  void fillRect(char, char, char, char, bool);
  void fillRoundRect(char, char, char, char, char, bool);
  void fillTriangle(char, char, char, char, char, char, bool);
  void drawBitmap(char, char, const char*, char, char, bool);
  void simb16x32(char, char, bool, char);
  void simb10x16(char, char, bool, char);
  void customObj(char, char, bool, char);
  void battery(uint8_t,uint8_t,uint8_t, bool);
  void signal (uint8_t,uint8_t,uint8_t);
  void printLcd(char*);
private:
  const uint8_t _rstpin, _cspin, _dinpin, _sckpin;
  
  void sendChar(char, char);
};
#endif
