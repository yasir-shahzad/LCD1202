#include "lcd1202.h"

/*!
  @brief Constructor for hardware SPI based on hardware controlled SCK (SCLK)
  and MOSI (DIN) pins. CS is still controlled by any IO pin. NOTE: MISO and SS
  will be set as an input and output respectively, so be careful sharing those
  pins!
  @param dc_pin   DC pin.
  @param cs_pin   CS pin
  @param rst_pin  RST pin
  @param theSPI   Pointer to SPIClass device for hardware SPI
 */
LCD1202::LCD1202(u8 rst_pin, u8 cs_pin, u8 din_pin, u8 sclk_pin) {
    _rstpin  = rst_pin;
    _cspin   = cs_pin;
    _dinpin = din_pin;
    _sckpin  = sclk_pin;
  }
 
//Clear the screen in LCD1202 RAM
void LCD1202::clearScreen() {
   int index;
    for (index = 0; index < 864 ; index++) {
      LCD_RAM[index] = (0x00);
    }
}

 
void LCD1202::sendChar(i8 mode, i8 c){
  digitalWrite(_cspin, 0);
  (mode)? digitalWrite(_dinpin,1) : digitalWrite(_dinpin,0);
  digitalWrite(_sckpin, 1);
  
  for(i8 i=0;i<8;i++) {
    digitalWrite(_sckpin,0);
    (c & 0x80)? digitalWrite(_dinpin,1) : digitalWrite(_dinpin,0);
    digitalWrite(_sckpin,1);
    c <<= 1;
  }

  digitalWrite(_sckpin, 0);
}

void LCD1202::display(){
  for(i8 p = 0; p < 9; p++){
    sendChar(LCD_C, SetYAddr| p); 
    sendChar(LCD_C, SetXAddr4);
    sendChar(LCD_C, SetXAddr3);

    for(i8 col=0; col < LCD_X; col++){
      sendChar(LCD_D, LCD_RAM[(LCD_X * p) + col]);
    }
  }
}


void LCD1202::initialize(){
  pinMode(_rstpin, OUTPUT);
  pinMode(_cspin,  OUTPUT);
  pinMode(_dinpin,  OUTPUT);
  pinMode(_sckpin, OUTPUT);

  digitalWrite(_cspin,  LOW);
  digitalWrite(_rstpin, HIGH);
  digitalWrite(_sckpin, LOW);
  digitalWrite(_dinpin,  LOW);
  
  delay(20);
  digitalWrite(_cspin, 1);

  sendChar(LCD_C,0x2F);            // Power control set(charge pump on/off)
  sendChar(LCD_C,0xA4);   
  sendChar(LCD_C,0xAF);            // экран вкл/выкл
  clearScreen();
  display();
}

void LCD1202::writePixel (i8 x, i8 y, bool color) {
  if ((x < 0) || (x >= LCD_X) || (y < 0) || (y >= LCD_Y)) return;

  if (color) LCD_RAM[x+ (y/8)*LCD_X] |= _BV(y%8);
  else       LCD_RAM[x+ (y/8)*LCD_X] &= ~_BV(y%8); 
}


void LCD1202::fillScreen(bool color) {
  fillRect(0, 0, LCD_X, LCD_Y, color);
}


void LCD1202::drawChar(i8 x, i8 y, bool color, u8 c) {

  if((x >= LCD_X) ||(y >= LCD_Y) || ((x + 4) < 0) || ((y + 7) < 0))
    return;

  if(c<128)            c = c-32;
  if(c>=144 && c<=175) c = c-48;
  if(c>=128 && c<=143) c = c+16;
  if(c>=176 && c<=191) c = c-48;
  if(c>191)  return;

  for (i8 i=0; i<6; i++ ) {
    i8 line;
    (i == 5)? line = 0x0 : line = pgm(BasicFont+(c*5)+i);
    for (i8 j = 0; j<8; j++) {
      (line & 0x1)? writePixel(x+i, y+j, color) : writePixel(x+i, y+j, !color);
      line >>= 1;
    }
  }
}


void LCD1202::print(i8 x, i8 y, bool color, char *str){
  unsigned char type = *str;
  if(type>=128) x=x-3;
  while(*str){ 
    drawChar(x, y, color, *str++);
    unsigned char type = *str;
    (type>=128)? x=x+3 : x=x+6;
  }
}

void LCD1202::print(i8 x, i8 y, bool color, long num){
  char c[20];
  print(x, y, color, ltoa(num,c,10));
}

void LCD1202::print_1607(i8 x, i8 y, bool color, char *str)
{
i8 nPos[16]={0,6,12,18,24,30,36,42,48,54,60,66,72,78,84,90};
i8 nStr[7]={1,10,20,30,40,50,60};
print(nPos[x], nStr[y], color, str);
}


void LCD1202::drawLine(i8 x0, i8 y0, i8 x1, i8 y1, bool color) {
  int steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    lcd1202_swap(x0, y0);
    lcd1202_swap(x1, y1);
  }
  if (x0 > x1) {
    lcd1202_swap(x0, x1);
    lcd1202_swap(y0, y1);
  }
  int dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int err = dx / 2;
  int ystep;

  (y0 < y1)?  ystep = 1 : ystep = -1;

  for (; x0<=x1; x0++) {
    (steep)? writePixel(y0, x0, color) : writePixel(x0, y0, color);
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


void LCD1202::drawFastVLine(i8 x, i8 y, i8 h, bool color) {
  drawLine(x, y, x, y+h-1, color);
}


void LCD1202::drawFastHLine(i8 x, i8 y, i8 w, bool color) {
  drawLine(x, y, x+w-1, y, color);
}


void LCD1202::drawRect(i8 x, i8 y, i8 w, i8 h, bool color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}





//void LCD1202::drawCircle(int xCenter, int yCenter, int radius,
//		     byte bGraphicsMode)
/*
* Draw or clear a circle of radius r at x,y centre
*/
void LCD1202::drawCircle(i8 x0, i8 y0, u16 r, bool color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  writePixel(x0, y0+r, color);
  writePixel(x0, y0-r, color);
  writePixel(x0+r, y0, color);
  writePixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    writePixel(x0 + x, y0 + y, color);
    writePixel(x0 - x, y0 + y, color);
    writePixel(x0 + x, y0 - y, color);
    writePixel(x0 - x, y0 - y, color);
    writePixel(x0 + y, y0 + x, color);
    writePixel(x0 - y, y0 + x, color);
    writePixel(x0 + y, y0 - x, color);
    writePixel(x0 - y, y0 - x, color);
  }
}

void LCD1202::drawRoundRect(i8 x, i8 y, i8 w, i8 h, i8 r, bool color) {
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

void LCD1202::drawTriangle(i8 x0, i8 y0, i8 x1, i8 y1, i8 x2, i8 y2, bool color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void LCD1202::drawCircleHelper(i8 x0, i8 y0, i8 r, i8 cornername, bool color) {
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
      writePixel(x0 + x, y0 + y, color);
      writePixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      writePixel(x0 + x, y0 - y, color);
      writePixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      writePixel(x0 - y, y0 + x, color);
      writePixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      writePixel(x0 - y, y0 - x, color);
      writePixel(x0 - x, y0 - y, color);
    }
  }
}

void LCD1202::fillCircle(i8 x0, i8 y0, i8 r, bool color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}


void LCD1202::fillCircleHelper(i8 x0, i8 y0, i8 r, i8 cornername, i8 delta, bool color) {

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


void LCD1202::fillRect(i8 x, i8 y, i8 w, i8 h, bool color) {
  
  for (u16 i=x; i<x+w; i++) {
    drawFastVLine(i, y, h, color);
  }
}


void LCD1202::fillRoundRect(i8 x, i8 y, i8 w, i8 h, i8 r, bool color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r , y+r, r, 2, h-2*r-1, color);
}


void LCD1202::fillTriangle(i8 x0, i8 y0, i8 x1, i8 y1, i8 x2, i8 y2, bool color) {

  int a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    lcd1202_swap(y0, y1); lcd1202_swap(x0, x1);
  }
  if (y1 > y2) {
    lcd1202_swap(y2, y1); lcd1202_swap(x2, x1);
  }
  if (y0 > y1) {
    lcd1202_swap(y0, y1); lcd1202_swap(x0, x1);
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

  u16
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

    if(a > b) lcd1202_swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }

  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if(a > b) lcd1202_swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }
}


void LCD1202::drawBitmap(i8 x, i8 y, const char *bitmap, i8 w, i8 h, bool color) {
  for (u16 j=0; j<h; j++) {
    for (u16 i=0; i<w; i++ ) {
      if (pgm(bitmap + i + (j/8)*w) & _BV(j%8)) {
        writePixel(x+i, y+j, color);
      }
    }
  }
}


void LCD1202::simb16x32(i8 x, i8 y, bool color, i8 c){
  for (i8 k=0;k<4;k++){
    for (i8 i=0;i<16;i++){
      i8 line = pgm(&(mass16x32[c][i+k*16]));
      for (i8 j = 0; j<8; j++) {
        (line & 0x01)? writePixel(x+i, y+j+k*8, color) : writePixel(x+i, y+j+k*8, !color);
        line >>= 1;
      }
    }
  }
}


void LCD1202::simb10x16(i8 x, i8 y, bool color, i8 c){
  for (i8 k=0;k<2;k++){
    for (i8 i=0;i<10;i++){
      i8 line = pgm(&(mass10x16[c][i+k*10]));
      for (i8 j = 0; j<8; j++) {
        (line & 0x01)? writePixel(x+i, y+j+k*8, color) : writePixel(x+i, y+j+k*8, !color);
        line >>= 1;
      }
    }
  }
}


void LCD1202::customObj(i8 x, i8 y, bool color, i8 c){
//  fillRect(x, y, 14, 12 , 0); display (); delay(200);
  for (i8 k=0;k<2;k++){  //i8 row
    for (i8 i=0;i<12;i++){
      i8 line = pgm(&(mass10x10[c][i+k*12]));
      for (i8 j = 0; j<8; j++) {
        (line & 0x01)? writePixel(x+i, y+j+k*8, color) : writePixel(x+i, y+j+k*8, !color);
        line >>= 1;
      }
    }
  }
 // display ();
}


void LCD1202::battery(u8 x, u8 y, u8 val, bool  charging){
    static bool chrg_cyc = true;   //charging cycle
    static u8 b_val;               //battery value(0 100%)
         
     if(charging) {
        if(chrg_cyc) {
            if(millis()-previousTime > 700) {
              b_val = (val < 5)? 0 : map(val, 5, 100, 1, 6);
              previousTime = millis();
              chrg_cyc = false;
            }
         }
        else if(millis()-previousTime > 700) {
           previousTime = millis();
           b_val += 1;
           if(b_val == 6) chrg_cyc = true;
        }
       if((millis()+100) < previousTime) 
         previousTime = 0;
     }
     else {
      b_val = (val < 5)? 0 : map(val, 5, 100, 1, 6);
      }

   //fillRect(x-2, y, 20, 12 , 0); //clear the lcd 
    drawRect(x, y, 18, 11, 1);
    drawRect(x-2, y+2, 3, 7, 1)  ;
    drawFastVLine(x, y+3, 5, 0);
             
     for (int i = 1, j = x+14 ; i <= b_val; i++, j -=3) {
       if(i <= 5) drawRect(j, y+2, 2, 7, 1);  
       else  drawFastVLine(x, y+4, 3, 1);     
     }
     //  display ();
}

void LCD1202::signal(u8 x, u8 y, u8 value){
    //   fillRect(x, y, 15, 12 , 0); //clear the lcd 
       drawFastHLine(x+1, y+1, 8, 1);
       drawRect(x+4, y+2, 2, 9, 1);
       drawLine(x,y+2,x+3,y+5,1);
       drawLine(x+6,y+5,x+9,y+2,1);
  
       for (int i = 10, j = x+7, k=-1 ; 
               i <= value; i+=20, j +=3, k-=2)
       drawRect(j, y+10, 2, k, 1);
   //    display();
}

void LCD1202::printLcd(char* message){
     clearScreen();
     battery(78,0,90,0);
     signal(0,0,0);
     print (12, 30, 1, message); 
     display (); 
}


