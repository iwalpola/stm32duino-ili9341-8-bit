/*
  ILI9431 Rudimentary library
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  Modified by Roger Clark. www.rogerclark.net for Maple mini 25th April 2015 , where the LED is on PB1
  
 */

uint8_t LCD_RST = PB8;
uint8_t LCD_CS = PB7;
uint8_t LCD_RS = PB6;
uint8_t LCD_WR = PB5;
uint8_t LCD_RD = PB4;
uint8_t DPINS[] = {PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7};
  
#define background Black
#define TFTWIDTH   240
#define TFTHEIGHT  320

void LCD_Init() {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i <= 7; i++)
    pinMode(DPINS[i], OUTPUT);
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
  digitalWrite(LCD_WR, LOW);
  digitalWrite(LCD_WR, HIGH);
}

void lcd_read_32() {
  uint8_t i;
  for (i = 0; i <= 7; i++)
    pinMode(DPINS[i], INPUT);
  digitalWrite(LCD_RD, LOW);
  delay(5);
  for (i = 0; i <= 7;i++) { 
    if (digitalRead(DPINS[7-i])==HIGH)
    Serial.print("1");
    else Serial.print("0");
  }
  digitalWrite(LCD_RD, HIGH);
  delay(5);
  digitalWrite(LCD_RD, LOW);
  delay(5);
  
  for (i = 0; i <= 7;i++) { 
    if (digitalRead(DPINS[7-i])==HIGH)
    Serial.print("1");
    else Serial.print("0");
  }
  digitalWrite(LCD_RD, HIGH);
  delay(5);
  digitalWrite(LCD_RD, LOW);
  delay(5);
  for (i = 0; i <= 7;i++) { 
    if (digitalRead(DPINS[7-i])==HIGH)
    Serial.print("1");
    else Serial.print("0");
  }
  digitalWrite(LCD_RD, HIGH);
  delay(5);
  digitalWrite(LCD_RD, LOW);
  delay(5);
  for (i = 0; i <= 7;i++) { 
    if (digitalRead(DPINS[7-i])==HIGH)
    Serial.print("1");
    else Serial.print("0");
  }
  digitalWrite(LCD_RD, HIGH);
  for (i = 0; i <= 7; i++)
    pinMode(DPINS[i], OUTPUT);
  return;
  
}

uint32_t lcd_read_id() {
  uint8_t i;
  uint32_t id;
  uint8_t temp=0;
  //SET INPUT OK
  for (i = 0; i <= 7; i++)
    pinMode(DPINS[i], INPUT);
  //BEGIN READ
  digitalWrite(LCD_RD, LOW);
  delay(5);
  for (i = 0; i <= 7;i++) { 
    id <<= 1;
    if (digitalRead(DPINS[7-i])==HIGH){
      Serial.print("1");
      id |= 1;
    }
    else Serial.print("0");
  }
  digitalWrite(LCD_RD, HIGH);
  delay(5);
  digitalWrite(LCD_RD, LOW);
  delay(5);
  
  for (i = 0; i <= 7;i++) { 
    id <<= 1;
    if (digitalRead(DPINS[7-i])==HIGH){
      Serial.print("1");
      id |= 1;
    }
    else Serial.print("0");
  }
  digitalWrite(LCD_RD, HIGH);
  delay(5);
  digitalWrite(LCD_RD, LOW);
  delay(5);
  for (i = 0; i <= 7;i++) { 
    id <<= 1;
    if (digitalRead(DPINS[7-i])==HIGH){
      Serial.print("1");
      id |= 1;
    }
    else Serial.print("0");
  }
  digitalWrite(LCD_RD, HIGH);
  delay(5);
  digitalWrite(LCD_RD, LOW);
  delay(5);
  for (i = 0; i <= 7;i++) { 
    id <<= 1;
    if (digitalRead(DPINS[7-i])==HIGH){
      Serial.print("1");
      id |= 1;
    }
    else Serial.print("0");
  }
  digitalWrite(LCD_RD, HIGH);
  for (i = 0; i <= 7; i++)
    pinMode(DPINS[i], OUTPUT);
  return id;
  
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

// the setup function runs once when you press reset or power the board
void setup() {
  //Begin serial
  Serial.begin(115200);
  // initialize LCD.
  LCD_Init();
}

// the loop function runs over and over again forever
void loop() {
  uint32_t id;
  //read ID
  digitalWrite(LCD_CS, LOW);
  lcd_cmd(0xD3);
  digitalWrite(LCD_RS, HIGH);
  Serial.println("BINARY ID");
  id = lcd_read_id();
  Serial.println("");
  Serial.println("HEX ID : ");
  Serial.println(id, HEX);
  digitalWrite(LCD_CS, HIGH);
  delay(5000);
  
}
