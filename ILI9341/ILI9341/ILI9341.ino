/*
  ILI9431 Rudimentary library
  Isuru Walpola
  
 */

#include "reg_9341.h"

uint8_t LCD_RST = PB8;
uint8_t LCD_CS = PB7;
uint8_t LCD_RS = PB6;
uint8_t LCD_WR = PB5;
uint8_t LCD_RD = PB4;
uint8_t DPINS[] = {PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7};

#define Black           0x0000
#define White           0xFFFF
#define Red             0xF800
#define Green           0x07E0
#define Blue            0x102E
#define Cyan            0x07FF
#define Magenta         0xF81F
#define Navy            0x000F
#define DarkGreen       0x03E0
#define Purple          0x780F
#define Olive           0x7BE0
#define DarkGrey        0x7BEF
#define DarkCyan        0x001F
#define Yellow          0xFFE0
#define Orange          0xFD20

uint16 indexed_color[] = { Black, White, Red, Green, Blue, Cyan, Magenta, Navy, DarkGreen, Purple, Olive, DarkGrey, DarkCyan, Yellow, Orange};
  
#define background Black
#define TFTWIDTH   240
#define TFTHEIGHT  320

uint8_t rotation, textsize;
uint8_t refreshaddress = 1;
int16_t
    _width, _height, // Display w/h as modified by current rotation
    cursor_x, cursor_y,
    textcolor, textbgcolor;

void LCD_Init() {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i <= 7; i++)
    pinMode(DPINS[i], OUTPUT);
  rotation  = 0;
  cursor_y  = cursor_x = 0;
  textsize  = 1;
  textcolor = 0xFFFF;//replace with BLACK later
  _width = TFTWIDTH;
  _height = TFTHEIGHT;
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(15);
  digitalWrite(LCD_RST, HIGH);
  delay(15);
  digitalWrite(LCD_CS, LOW);
  lcd_cmd_data(LCD_SWRESET, 0x00);
  lcd_cmd_data(LCD_DISPLAY_OFF, 0x00);
  lcd_cmd_data(LCD_POWER1, 0x23);
  lcd_cmd_data(LCD_POWER2, 0x10);
  lcd_cmd(LCD_VCOM1);
  lcd_data(0x2B);
  lcd_data(0x2B);
  lcd_cmd_data(LCD_VCOM2, 0xC0);
  lcd_cmd_data(LCD_MAC, (1<<3)|(1<<7));//((1<<3)|(1<<6)); //rotate 180
  lcd_cmd_data(LCD_PIXEL_FORMAT, 0x55);
  lcd_cmd(LCD_FRMCTR1);
  lcd_data(0x00);
  lcd_data(0x1B);
  lcd_cmd_data(LCD_ETMOD, 0x07);
  lcd_cmd_data(LCD_SLEEP_OUT, 0x00);
  delay(150);
  lcd_cmd_data(LCD_DISPLAY_ON, 0x00);
  delay(500);
  //lcd_cmd_data(LCD_DINVOFF, 0x00);
  delay(500);
  digitalWrite(LCD_CS, HIGH);
  setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
  //setWindows(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
  return;
}

void reset(void) {

  digitalWrite(LCD_CS, HIGH); //CS_IDLE
//  CD_DATA;
  digitalWrite(LCD_WR, HIGH); //WR_IDLE
  digitalWrite(LCD_RD, HIGH); //RD_IDLE


  if(LCD_RST) {
    digitalWrite(LCD_RST, LOW);
    delay(2);
    digitalWrite(LCD_RST, HIGH);
  }

  // Data transfer sync
  digitalWrite(LCD_CS, LOW); //CS_ACTIVE
  lcd_cmd(0x00);
  for(uint8_t i=0; i<3; i++) WR_STROBE(); // Three extra 0x00s
  digitalWrite(LCD_CS, HIGH); //CS_IDLE
}

void lcd_write_bus(uint8_t val) {
  unsigned char i, temp, data;
  data = val;
  for (i = 0; i <= 7; i++)
    pinMode(DPINS[i], OUTPUT);
  
  for (i = 0; i <= 7;i++) { 
    temp = (data & 0x01);
    if (temp)
      digitalWrite(DPINS[i], HIGH);
    else
      digitalWrite(DPINS[i], LOW);   
    data = data >> 1;
  }
  WR_STROBE();
}

void lcd_cmd(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  lcd_write_bus(cmd);
}

void lcd_data(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  lcd_write_bus(data);
}

void lcd_cmd_data(uint8_t cmd, uint8_t data) {
  lcd_cmd(cmd);
  lcd_data(data);
}

void setAddrWindow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {

  digitalWrite(LCD_CS, LOW);//CS_ACTIVE
  lcd_cmd(LCD_COLUMN_ADDR); // Set_column_address 4 parameters
  lcd_data(0x00);
  lcd_data(x1);   
  lcd_data(0x00);
  lcd_data(x2);   
  lcd_cmd(LCD_PAGE_ADDR); // Set_page_address 4 parameters
  lcd_data(0x00);
  lcd_data(y1);   
  lcd_data(0x00);
  lcd_data(y2);   
  digitalWrite(LCD_CS, HIGH); //CS_IDLE
  //lcd_cmd(0x2c); // Write_memory_start
}

void flood(uint16_t color, uint32_t len) {
  uint16_t blocks;
  uint8_t  i, hi = color >> 8,
              lo = color;
  digitalWrite(LCD_CS, LOW);//CS_ACTIVE
  lcd_cmd(LCD_GRAM);

  // Write first pixel normally, decrement counter by 1
  lcd_data(hi);
  lcd_data(lo);
  len--;

  blocks = (uint16_t)(len / 64); // 64 pixels/block

  if(hi == lo) {
    // High and low bytes are identical.  Leave prior data
    // on the port(s) and just toggle the write strobe.
    while(blocks--) {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do {
        WR_STROBE(); WR_STROBE(); WR_STROBE(); WR_STROBE(); // 2 bytes/pixel
        WR_STROBE(); WR_STROBE(); WR_STROBE(); WR_STROBE(); // x 4 pixels
      } while(--i);
    }
    // Fill any remaining pixels (1 to 64)
    for(i = (uint8_t)len & 63; i--; ) {
      WR_STROBE();
      WR_STROBE();
    }
  } else {
    while(blocks--) {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do {
        lcd_data(hi); lcd_data(lo); lcd_data(hi); lcd_data(lo);
        lcd_data(hi); lcd_data(lo); lcd_data(hi); lcd_data(lo);
      } while(--i);
    }
    for(i = (uint8_t)len & 63; i--; ) {
      lcd_data(hi);
      lcd_data(lo);
    }
  }
  digitalWrite(LCD_CS, HIGH);//CS_IDLE
}

void drawFastHLine(int16_t x, int16_t y, int16_t length,
  uint16_t color)
{
  int16_t x2;

  // Initial off-screen clipping
  if((length <= 0     ) ||
     (y      <  0     ) || ( y                  >= _height) ||
     (x      >= _width) || ((x2 = (x+length-1)) <  0      )) return;

  if(x < 0) {        // Clip left
    length += x;
    x       = 0;
  }
  if(x2 >= _width) { // Clip right
    x2      = _width - 1;
    length  = x2 - x + 1;
  }

  setAddrWindow(x, y, x2, y);
  flood(color, length);
  if(refreshaddress) setAddrWindow(0, 0, _width - 1, _height - 1);
  else                  setLR();
}

void setLR(void){
return;
}

void drawFastVLine(int16_t x, int16_t y, int16_t length,
  uint16_t color)
{
  int16_t y2;

  // Initial off-screen clipping
  if((length <= 0      ) ||
     (x      <  0      ) || ( x                  >= _width) ||
     (y      >= _height) || ((y2 = (y+length-1)) <  0     )) return;
  if(y < 0) {         // Clip top
    length += y;
    y       = 0;
  }
  if(y2 >= _height) { // Clip bottom
    y2      = _height - 1;
    length  = y2 - y + 1;
  }

  setAddrWindow(x, y, x, y2);
  flood(color, length);
  if(refreshaddress) setAddrWindow(0, 0, _width - 1, _height - 1);
  else                  setLR();
}

void fillRect(int16_t x1, int16_t y1, int16_t w, int16_t h, 
  uint16_t fillcolor) {
  int16_t  x2, y2;

  // Initial off-screen clipping
  if( (w            <= 0     ) ||  (h             <= 0      ) ||
      (x1           >= _width) ||  (y1            >= _height) ||
     ((x2 = x1+w-1) <  0     ) || ((y2  = y1+h-1) <  0      )) return;
  if(x1 < 0) { // Clip left
    w += x1;
    x1 = 0;
  }
  if(y1 < 0) { // Clip top
    h += y1;
    y1 = 0;
  }
  if(x2 >= _width) { // Clip right
    x2 = _width - 1;
    w  = x2 - x1 + 1;
  }
  if(y2 >= _height) { // Clip bottom
    y2 = _height - 1;
    h  = y2 - y1 + 1;
  }
  setAddrWindow(x1, y1, x2, y2);
  flood(fillcolor, (uint32_t)w * (uint32_t)h);
  if(refreshaddress) setAddrWindow(0, 0, _width - 1, _height - 1);
  else                  setLR();
}

void WR_STROBE(void){
  digitalWrite(LCD_WR, LOW);
  digitalWrite(LCD_WR, HIGH);
}

void fillScreen(uint16_t color) {
  setAddrWindow(0, 0, _width - 1, _height - 1);
  flood(color, (long)TFTWIDTH * (long)TFTHEIGHT);
}

void drawPixel(int16_t x, int16_t y, uint16_t color) {

  // Clip
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

  digitalWrite(LCD_CS, LOW);//CS_ACTIVE
  setAddrWindow(x, y, _width-1, _height-1);
  lcd_cmd(LCD_GRAM);
  lcd_data(color >> 8); lcd_data(color);

  digitalWrite(LCD_CS, HIGH);//CS_IDLE
}

// Issues 'raw' an array of 16-bit color values to the LCD; used
// externally by BMP examples.  Assumes that setWindowAddr() has
// previously been set to define the bounds.  Max 255 pixels at
// a time (BMP examples read in small chunks due to limited RAM).
void pushColors(uint16_t *data, uint8_t len, boolean first) {
  uint16_t color;
  uint8_t  hi, lo;
  digitalWrite(LCD_CS, LOW);//CS_ACTIVE
  if(first == true) { // Issue GRAM write command only on first call
    lcd_cmd(LCD_GRAM);
  }
  while(len--) {
    color = *data++;
    hi    = color >> 8; // Don't simplify or merge these
    lo    = color;      // lines, there's macro shenanigans
    lcd_data(hi);         // going on.
    lcd_data(lo);
  }
  digitalWrite(LCD_CS, HIGH);//CS_IDLE
}

void setRotation(uint8_t x) {
  // Then perform hardware-specific rotation operations...
  rotation = x;

  digitalWrite(LCD_CS, LOW);//CS_ACTIVE
  // MEME, HX8357D uses same registers as 9341 but different values
   uint8_t t;

   switch (rotation) {
   case 2:
     t = ILI9341_MADCTL_MX | ILI9341_MADCTL_RGB;
     break;
   case 3:
     t = ILI9341_MADCTL_MV | ILI9341_MADCTL_RGB;
     break;
  case 0:
    t = ILI9341_MADCTL_MY | ILI9341_MADCTL_RGB;
    break;
   case 1:
     t = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_RGB;
     break;
  }
   lcd_cmd_data(LCD_MAC, t ); // MADCTL
   // For 9341, init default full-screen address window:
   setAddrWindow(0, 0, _width - 1, _height - 1); // CS_IDLE happens here 
}


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin PB1 as an output.
  LCD_Init();
}

// the loop function runs over and over again forever
void loop() {
  uint8_t order[] = {0,2,3,1};
  for(uint8_t i = 0; i<=3 ; i++){
    setRotation(order[i]);
  delay(1000);
  fillScreen(Magenta);
  delay(1000);
  fillRect(0, 0, 20, 10, Cyan);
  delay(2000);
  fillRect(0, TFTHEIGHT-10, 20, 10, Cyan);
  delay(2000);  
  fillRect(TFTWIDTH-20, TFTHEIGHT-10, 20, 10, Cyan);
  delay(2000);
  fillRect(TFTWIDTH-20, 0, 20, 10, Cyan);
  delay(2000);
  fillScreen(Cyan);}
}
