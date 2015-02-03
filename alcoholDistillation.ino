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
int relaysOnCounter = 0;
boolean getRelayState(const int index){
  return channelRelayStates[index];
}

void setRelayState(const int index, const int state){
  channelRelayStates[index] = state;
}

void setRelay(const int index,const boolean state){
  if(getRelayState(index) != state){
    digitalWrite(channelRelayPorts[index], state);
    if(state == RELAY_ON){
      relaysOnCounter++;
    }
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

void turnOffAllRelays(){
  for (int i = 0; i < NUMBER_OF_RELAYS; i++){
    setRelay(i, RELAY_OFF);
  }
}

void turnOnAllRelays(){
  for (int i = 0; i < NUMBER_OF_RELAYS; i++){
    setRelay(i, RELAY_ON);
  }
}

int getNumberOfOnRelays(){
  int sum = 0; 

  for (int i = 0; i < NUMBER_OF_RELAYS; i++){

    if(getRelayState(i) == RELAY_ON){
      sum++;
    }
  }
  return sum;
}

void turnOnOneRelay(){
  if(getNumberOfOnRelays() != 1 ){
    turnOffAllRelays();
    setRelay(relaysOnCounter % NUMBER_OF_RELAYS, RELAY_ON);
  }
}

#include <Button.h>
/*
 * Tact Switch -> 7
 */
const int STATE_BUTTON_PIN = 7;
Button button = Button(STATE_BUTTON_PIN, PULLUP);



int stage = 0;
void checkButtonStage(){
  if(button.uniquePress()){
    stage++;
  }
}

void lcdInformation(int s, float t){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Stage:");
  lcd.print(s);
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.print(t);
}

void stage_0(){  
  turnOffAllRelays();
  lcd.setCursor(0, 0);
  lcd.print("Push the button");
  lcd.setCursor(0,1);
  lcd.print("to start.");
}

const float METHANOL_BOILING_POINT = 25.6;
void stage_1(){
  float temperature = sensors.getTempCByIndex(0);

  if(temperature < METHANOL_BOILING_POINT){
    turnOnAllRelays();
  }
  else{
    stage++;
  }
  lcdInformation(stage, temperature);
}

void stage_2(){
  float temperature = sensors.getTempCByIndex(0);

  if(temperature <  METHANOL_BOILING_POINT - 1){
    turnOnOneRelay();
  }
  else{
    if(temperature > METHANOL_BOILING_POINT + 1){
      turnOffAllRelays();
    }
  }
  lcdInformation(stage, temperature);
}




void setup(){
  lcd.begin(LCD_COLS, LCD_ROWS);
  sensors.begin();
  setupRelays();
}

void loop(){
  checkButtonStage();
  sensors.requestTemperatures();
  switch(stage){
  case 0:
    stage_0();
    break;
  case 1:
    stage_1();
    break;
  case 2:
    stage_2();
    break;
  }
}





















