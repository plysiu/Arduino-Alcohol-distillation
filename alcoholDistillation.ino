/*  LCD LCM1602
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
/*
 * Temperature sensor DS18B20
 * ONE_WIRE_BUS -> 6
 */
#include <OneWire.h>
#include <DallasTemperature.h>
const int ONE_WIRE_BUS = 6;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); 

void setup(){
  lcd.begin(LCD_COLS, LCD_ROWS);
  sensors.begin();
}

void loop(){
  sensors.requestTemperatures();
  lcd.setCursor(0,0);
  lcd.print(sensors.getTempCByIndex(0));
}
