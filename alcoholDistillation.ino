#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Button.h>
/*  
 * LCD LCM1602
 * VCC -> 5V
 * GND -> GND
 * SCL -> A5
 * SDA -> A4
 */
const int LCD_ROWS = 2;
const int LCD_COLS = 16;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
/*
 * Temperature sensor DS18B20
 * ONE_WIRE_BUS -> 6
 */
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

int channelRelayPorts[NUMBER_OF_RELAYS] = { 
  IN0, IN1, IN2, IN3 };
boolean channelRelayStates[NUMBER_OF_RELAYS] = { 
  RELAY_OFF, RELAY_OFF, RELAY_OFF, RELAY_OFF };
int relaysOnCounter = 0;

boolean getRelayState(const int index){
  return channelRelayStates[index];
}

void setRelayState(const int index, const int state){
  channelRelayStates[index] = state;
}

void setRelay(const int index, const boolean state){
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
  digitalWrite(channelRelayPorts[index], channelRelayStates[index]);

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

/*
 * Tact Switch -> 7
 * Tact Switch -> 8
 */
const int STAGE_UP_BUTTON_PIN = 7;
const int STAGE_DOWN_BUTTON_PIN = 8;

Button stageUp = Button(STAGE_UP_BUTTON_PIN, PULLUP);
Button stageDown = Button(STAGE_DOWN_BUTTON_PIN, PULLUP);

int stage = 0;
void checkButtonStage(){
  if(stageUp.uniquePress()){
    stage++;
  }
  if(stageDown.uniquePress()){
    stage--;
  }
}

void lcdInformation(float t){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Stage:");
  lcd.print(stage);
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.print(t);
  lcd.setCursor(12, 1);
  for(int i = 0; i < NUMBER_OF_RELAYS; i++){
    lcd.print(!getRelayState(i));
  }
}

const int GRADIENT = 1;
const float METHANOL_BOILING_POINT = 64.6;
const float ETHANOL_BOILING_POINT = 78.38;
const float THIRD_BOILING_POINT = 83;

void heating(const float currentTemperature, const float temperatureTreshold){
  if(currentTemperature < temperatureTreshold){
    turnOnAllRelays();
  }
  else{
    stage++;
  }
}
void maintainingTheTemperature(const float currentTemperature, const float temperatureToMaintain){
  if(currentTemperature <  temperatureToMaintain - GRADIENT){
    turnOnOneRelay();
  }
  else{
    if(currentTemperature > temperatureToMaintain + GRADIENT){
      turnOffAllRelays();
    }
  }
}

void setup(){
  lcd.begin(LCD_COLS, LCD_ROWS);
  sensors.begin();
  setupRelays();
  turnOffAllRelays();
}
float temperature = 0;

void loop(){

  checkButtonStage();
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  switch(stage){
  case 0:
    turnOffAllRelays();
    break;
  case 1:
    heating(temperature, METHANOL_BOILING_POINT);
    break;
  case 2:
    maintainingTheTemperature(temperature, METHANOL_BOILING_POINT);
    break;
  case 3:
    heating(temperature, ETHANOL_BOILING_POINT);
    break;
  case 4:
    maintainingTheTemperature(temperature, ETHANOL_BOILING_POINT);
    break;
  case 5:
    heating(temperature, THIRD_BOILING_POINT);
    break;
  case 6:
    maintainingTheTemperature(temperature, THIRD_BOILING_POINT);
    break;
  default :
    turnOffAllRelays();
    break;
  }
  lcdInformation(temperature);
}



















