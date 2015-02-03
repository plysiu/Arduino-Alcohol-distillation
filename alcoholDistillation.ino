/*  
 * LCD LCM1602
 * VCC -> 5V
 * GND -> GND
 * SCL -> A5
 * SDA -> A4
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
/*
 * 4 x Channel Relay SRD-05VDC-SL-C
 * IN0 -> 2
 * IN1 -> 3
 * IN2 -> 4
 * IN3 -> 5
 */
const int IN0 = 2;
const int IN1 = 3;
const int IN2 = 4;
const int IN3 = 5; 
const int NUMBER_OF_RELAYS = 4;
const boolean RELAY_ON = LOW;
const boolean RELAY_OFF = HIGH;
int channelRelayPorts[] = {
  IN0,IN1,IN2,IN3};
boolean channelRelayStates[] = {
  LOW,LOW,LOW,LOW};
  
 

boolean getRelayState(const int index){
  return channelRelayStates[index];
}

void setRelayState(const int index, const int state){
  channelRelayStates[index] = state;
}

void setRelay(const int index,const boolean state){
  if(getRelayState(index) != state){
    digitalWrite(channelRelayPorts[index], state);
    setRelayState(index, state);
  }
}

void setupRelay(const int index){
  pinMode(channelRelayPorts[index], OUTPUT);
  setRelay(index, RELAY_OFF);
}

void setupRelays(){
  for(int i = 0; i < NUMBER_OF_RELAYS; i++){
    setupRelay(i);
  }
}

void setup(){
  lcd.begin(LCD_COLS, LCD_ROWS);
  sensors.begin();
  setupRelays();
}

void loop(){
  
  if(getRelayState(0) == RELAY_OFF)
    setRelay(0,RELAY_ON);
  
  sensors.requestTemperatures();
  lcd.setCursor(0,0);
  lcd.print(sensors.getTempCByIndex(0));
}







