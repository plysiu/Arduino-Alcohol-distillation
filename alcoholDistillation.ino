
/*  LCD model LCM1602
 *  VCC -> 5V
 *  GND -> GND
 *  SCL -> A5
 *  SDA -> A4
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
const int LCD_ROWS = 2;
const int LCD_COLS = 16;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup(){
  lcd.begin(LCD_COLS, LCD_ROWS);
}

void loop(){
  lcd.setCursor(0,0);
  lcd.print("Hello World!");
}
