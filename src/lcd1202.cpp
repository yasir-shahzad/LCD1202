#include "Arduino.h"
//#include <avr/pgmspace.h>
#include "lcd1202.h"
#include "font.h"


#define LCD_X        96
#define LCD_Y        68
#define LCD_String    9

#define W   94
#define H   66

#define SetYAddr   0xB0
#define SetXAddr4  0x00
#define SetXAddr3  0x10

#define pgm     pgm_read_byte
#define swap(a, b) { int t = a; a = b; b = t; }

#define LCD_D         1
#define LCD_C         0

byte LCD_RAM[LCD_X*LCD_String];

volatile uint8_t RES, CS, Data, Clock;


LCD1202::LCD1202(uint8_t _RES, uint8_t _CS, uint8_t _Data, uint8_t _Clock) {
    RES = _RES;
    CS = _CS;
    Data = _Data;
    Clock = _Clock;
  }
 

void LCD1202::clearLcd() {
  for (int index = 0; index < 864 ; index++){
     LCD_RAM[index] = (0x00);
  }
}

void LCD1202::dWrite(byte pin, byte val){
  digitalWrite(pin, val);
 // byte bit = digitalPinToBitMask(pin);
//  volatile byte *out;
//  out = portOutputRegister(digitalPinToPort(pin));
 // (val)? *out |= bit : *out &=~bit;
}

 
void LCD1202::sendChar(byte mode, byte c){
  dWrite(CS, 0);
  (mode)? dWrite(Data,1) : dWrite(Data,0);
  dWrite(Clock, 1);
  
  for(byte i=0;i<8;i++) {
    dWrite(Clock,0);
    (c & 0x80)? dWrite(Data,1) : dWrite(Data,0);
    dWrite(Clock,1);
    c <<= 1;
  }

  dWrite(Clock, 0);
}

void LCD1202::update(){
  for(byte p = 0; p < 9; p++){
    sendChar(LCD_C, SetYAddr| p); 
    sendChar(LCD_C, SetXAddr4);
    sendChar(LCD_C, SetXAddr3);

    for(byte col=0; col < LCD_X; col++){
      sendChar(LCD_D, LCD_RAM[(LCD_X * p) + col]);
    }
  }
}


void LCD1202::initialize(){
  pinMode(RES,   OUTPUT);
  pinMode(CS,    OUTPUT);
  pinMode(Data,  OUTPUT);
  pinMode(Clock, OUTPUT);

  dWrite(RES, 1);
  dWrite(Clock, 0);
  dWrite(Data, 0);
  dWrite(CS, 0);
  delay(20);
  dWrite(CS, 1);

  sendChar(LCD_C,0x2F);            // Power control set(charge pump on/off)
  sendChar(LCD_C,0xA4);   
  sendChar(LCD_C,0xAF);            // экран вкл/выкл
  clearLcd();
  update();
}

void LCD1202::drawPixel (byte x, byte y, boolean color) {
  if ((x < 0) || (x >= LCD_X) || (y < 0) || (y >= LCD_Y)) return;

  if (color) LCD_RAM[x+ (y/8)*LCD_X] |= _BV(y%8);
  else       LCD_RAM[x+ (y/8)*LCD_X] &= ~_BV(y%8); 
}


void LCD1202::fillScreen(boolean color) {
  fillRect(0, 0, LCD_X, LCD_Y, color);
}


void LCD1202::drawChar(byte x, byte y, boolean color, unsigned char c) {

  if((x >= LCD_X) ||(y >= LCD_Y) || ((x + 4) < 0) || ((y + 7) < 0))
    return;

  if(c<128)            c = c-32;
  if(c>=144 && c<=175) c = c-48;
  if(c>=128 && c<=143) c = c+16;
  if(c>=176 && c<=191) c = c-48;
  if(c>191)  return;

  for (byte i=0; i<6; i++ ) {
    byte line;
    (i == 5)? line = 0x0 : line = pgm(font+(c*5)+i);
    for (byte j = 0; j<8; j++) {
      (line & 0x1)? drawPixel(x+i, y+j, color) : drawPixel(x+i, y+j, !color);
      line >>= 1;
    }
  }
}


void LCD1202::print(byte x, byte y, boolean color, char *str){
  unsigned char type = *str;
  if(type>=128) x=x-3;
  while(*str){ 
    drawChar(x, y, color, *str++);
    unsigned char type = *str;
    (type>=128)? x=x+3 : x=x+6;
  }
}

void LCD1202::print(byte x, byte y, boolean color, long num){
  char c[20];
  print(x, y, color, ltoa(num,c,10));
}

void LCD1202::print_1607(byte x, byte y, boolean color, char *str)
{
byte nPos[16]={0,6,12,18,24,30,36,42,48,54,60,66,72,78,84,90};
byte nStr[7]={1,10,20,30,40,50,60};
print(nPos[x], nStr[y], color, str);
}


void LCD1202::drawLine(byte x0, byte y0, byte x1, byte y1, boolean color) {
  int steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }
  int dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int err = dx / 2;
  int ystep;

  (y0 < y1)?  ystep = 1 : ystep = -1;

  for (; x0<=x1; x0++) {
    (steep)? drawPixel(y0, x0, color) : drawPixel(x0, y0, color);
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


void LCD1202::drawFastVLine(byte x, byte y, byte h, boolean color) {
  drawLine(x, y, x, y+h-1, color);
}


void LCD1202::drawFastHLine(byte x, byte y, byte w, boolean color) {
  drawLine(x, y, x+w-1, y, color);
}


void LCD1202::drawRect(byte x, byte y, byte w, byte h, boolean color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}


void LCD1202::drawCircle(byte x0, byte y0, int16_t r, boolean color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void LCD1202::drawRoundRect(byte x, byte y, byte w, byte h, byte r, boolean color) {
  // smarter version
  drawFastHLine(x+r , y , w-2*r, color); // Top
  drawFastHLine(x+r , y+h-1, w-2*r, color); // Bottom
  drawFastVLine( x , y+r , h-2*r, color); // Left
  drawFastVLine( x+w-1, y+r , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r , y+r , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r , y+h-r-1, r, 8, color);
}

void LCD1202::drawTriangle(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2, boolean color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void LCD1202::drawCircleHelper(byte x0, byte y0, byte r, byte cornername, boolean color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void LCD1202::fillCircle(byte x0, byte y0, byte r, boolean color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}


void LCD1202::fillCircleHelper(byte x0, byte y0, byte r, byte cornername, byte delta, boolean color) {

  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}


void LCD1202::fillRect(byte x, byte y, byte w, byte h, boolean color) {
  
  for (int16_t i=x; i<x+w; i++) {
    drawFastVLine(i, y, h, color);
  }
}


void LCD1202::fillRoundRect(byte x, byte y, byte w, byte h, byte r, boolean color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r , y+r, r, 2, h-2*r-1, color);
}


void LCD1202::fillTriangle(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2, boolean color) {

  int a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a) a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a) a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa = 0,
    sb = 0;

  if(y1 == y2) last = y1; // Include y1 scanline
  else last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
*/
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }

  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
*/
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }
}


void LCD1202::drawBitmap(byte x, byte y, const char *bitmap, byte w, byte h, boolean color) {
  for (int16_t j=0; j<h; j++) {
    for (int16_t i=0; i<w; i++ ) {
      if (pgm(bitmap + i + (j/8)*w) & _BV(j%8)) {
        drawPixel(x+i, y+j, color);
      }
    }
  }
}


void LCD1202::simb16x32(byte x, byte y, boolean color, byte c){
  for (byte k=0;k<4;k++){
    for (byte i=0;i<16;i++){
      byte line = pgm(&(mass16x32[c][i+k*16]));
      for (byte j = 0; j<8; j++) {
        (line & 0x01)? drawPixel(x+i, y+j+k*8, color) : drawPixel(x+i, y+j+k*8, !color);
        line >>= 1;
      }
    }
  }
}


void LCD1202::simb10x16(byte x, byte y, boolean color, byte c){
  for (byte k=0;k<2;k++){
    for (byte i=0;i<10;i++){
      byte line = pgm(&(mass10x16[c][i+k*10]));
      for (byte j = 0; j<8; j++) {
        (line & 0x01)? drawPixel(x+i, y+j+k*8, color) : drawPixel(x+i, y+j+k*8, !color);
        line >>= 1;
      }
    }
  }
}


void LCD1202::customObj(byte x, byte y, boolean color, byte c){
//  fillRect(x, y, 14, 12 , 0); update (); delay(200);
  for (byte k=0;k<2;k++){  //byte row
    for (byte i=0;i<12;i++){
      byte line = pgm(&(mass10x10[c][i+k*12]));
      for (byte j = 0; j<8; j++) {
        (line & 0x01)? drawPixel(x+i, y+j+k*8, color) : drawPixel(x+i, y+j+k*8, !color);
        line >>= 1;
      }
    }
  }
 // update ();
}


void LCD1202::battery(uint8_t x, uint8_t y, uint8_t val, bool  charging){
        
        static bool charCyc = true;
        static uint8_t bVal;
         
        if(charging) {
            if(charCyc){
                if(millis()-previousTime > 700){
                bVal = (val < 5)? 0 : map(val, 5, 100, 1, 6);
                previousTime = millis();
                charCyc = false;
                }
             }
            else if(millis()-previousTime > 700) {
                previousTime = millis();
                bVal += 1;
                if(bVal == 6) charCyc = true;
            }
           if((millis()+100) < previousTime) previousTime = 0;
         }
        else {
          bVal = (val < 5)? 0 : map(val, 5, 100, 1, 6);
          }

   //    fillRect(x-2, y, 20, 12 , 0); //clear the lcd 
       drawRect(x, y, 18, 11, 1);
       drawRect(x-2, y+2, 3, 7, 1)  ;
       drawFastVLine(x, y+3, 5, 0);
             
  for (int i = 1, j = x+14 ; i <= bVal; i++, j -=3){
       if(i <= 5) drawRect(j, y+2, 2, 7, 1);  
       else  drawFastVLine(x, y+4, 3, 1);
     
  }
     //  update ();
}




void LCD1202::signal(uint8_t x, uint8_t y, uint8_t value){
    //   fillRect(x, y, 15, 12 , 0); //clear the lcd 
       drawFastHLine(x+1, y+1, 8, 1);
       drawRect(x+4, y+2, 2, 9, 1);
       drawLine(x,y+2,x+3,y+5,1);
       drawLine(x+6,y+5,x+9,y+2,1);
  
       for (int i = 10, j = x+7, k=-1 ; 
               i <= value; i+=20, j +=3, k-=2)
       drawRect(j, y+10, 2, k, 1);
   //    update();
}

void LCD1202::tick (uint8_t x ,uint8_t y){
  //  fillRect(x, y, 10, 10, 0); //clear the lcd  
    drawFastVLine(x+1, y+6, 1, 1); delay((20));update();
    drawFastVLine(x+2, y+6, 2, 1); delay((20));update();
    drawFastVLine(x+3, y+7, 2, 1); delay((20));update();
    drawFastVLine(x+4, y+6, 3, 1); delay((20));update();     
    drawFastVLine(x+5, y+4, 3, 1); delay((20));update();
    drawFastVLine(x+6, y+3, 2, 1); delay((20));update();
    drawFastVLine(x+7, y+1, 3, 1); delay((20));update();
    drawFastVLine(x+8, y, 2, 1);   delay((20));update();
    drawFastVLine(x+9, y, 1, 1); //update();
}
void LCD1202::printLcd(char* message){
     clearLcd();
     battery(78,0,90,0);
     signal(0,0,0);
     print (12, 30, 1, message); 
     update (); 
}


