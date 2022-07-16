
/*
 * Copyright (c) 2022 by Yasir Shahzad <Yasirshahzad918@gmail.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */


#ifndef LCD1202_H
#define LCD1202_H
#include <Arduino.h>


//Alias of Basic Types
#define i32 long int
#define i16 int
#define i8  char
#define u32 unsigned long int
#define u16 unsigned int
#define u8  unsigned char



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

       void sendChar(i8 mode, i8 c);
};
#endif
