/**
 * LCD LCM1602
 * VCC -> 5V
 * GND -> GND
 * SCL -> A5
 * SDA -> A4
 *
 * Tact Switch -> 2
 * Tact Switch -> 3
 *
 * 4 x Channel Relay SRD-05VDC-SL-C
 * IN4 -> 4
 * IN3 -> 5
 * IN2 -> 6
 * IN1 -> 7
 *
 * Temperature sensor DS18B20
 * ONE_WIRE_BUS -> 8
 *
 * METANOL = 64.6
 * ETANOL = 78.38
 * THIRD = 95
 */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>
#include <StandardCplusplus.h>
#include <serstream>
#include <vector>
#include <iterator>
//
#include <Stage.h>
#include <Relay.h>
#include <Hysteresis.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Button.h>


/**
 *
 */
OneWire oneWire(8);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

Hysteresis hysteresis;
float temperatures[4] = {64.6, 78.38, 85, 95};
float GRADIENT = 0.5;
long  iterator = 0;
const int numberOfRelays = 3;
const uint8_t relaysPins[numberOfRelays] = {
  4  , 5  , 6  //,7
};
std::vector<Relay> relays;

int additionalHeater = 0;
/**
 *
 */
void heating(float temperatureTreshold) {
  additionalHeater = 0;
  if (hysteresis.lastValue() < temperatureTreshold) {
    Relay::applyToAll(relays, Relay::Status::ON);
  }
  else {
    Stage::increase();
  }
}
/**
 *
 */
void maintaining(float temperatureToMaintain) {

  if (hysteresis.lastValue() <  temperatureToMaintain - GRADIENT) {
    // turnOnOneRelay();
    /**
     *  when is not rising tapply to a additional heater
     */
    iterator++;
    if (iterator % 10) {
      if (!hysteresis.isRising()) {
        additionalHeater=1;
      } else {
        additionalHeater=0;
      }
    }

    Relay::applyTo(relays, Relay::Status::ON, 1+ additionalHeater);
  }
  else {
    if (hysteresis.lastValue()  > temperatureToMaintain + GRADIENT) {
      Relay::applyToAll(relays, Relay::Status::OFF);
    }
  }
}
/**
 *
 */
void refreshDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Stage::value);
  lcd.print("|");
  lcd.setCursor(2, 0);
  lcd.print(millis() / 60000);
  lcd.setCursor(13, 0);
  for (int i = 0; i < relays.size(); i++) {
    lcd.print(Relay::Status::ON == relays.at(i).getStatus() ? true : false);
  }

  lcd.setCursor(0, 1);
  lcd.print(hysteresis.lastValue());
  if (Stage::value > 0 ) {
    lcd.setCursor(6, 1);
    lcd.print(temperatures[ (int)floor(  (Stage::value - 1) / 2)  ]);
    lcd.print("+-");
    lcd.print(GRADIENT);
  }


  Serial.print(millis() / 60000);
  Serial.print(" Stage:" );
  Serial.print(Stage::value);
  Serial.print( " Temperature:");
  Serial.print(hysteresis.lastValue());
  for (int i = 0; i < relays.size(); i++) {
    Serial.print(" Relay:");
    Serial.print(i + 1);
    Serial.print(" Pin:");
    Serial.print(relays.at(i).getPin());
    Serial.print(" Status:");
    Serial.print(Relay::Status::ON == relays.at(i).getStatus() ? "ON" : "OFF");
    Serial.print(" ");

    Serial.print(hysteresis.isRising() ? "YES" : "NO");
  }
  Serial.println();
}
/**
 *
 */
void setup(void) {
  Serial.begin(9600);
  Serial.println("SETUP:START");
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < numberOfRelays; i++) {
    relays.push_back(Relay(relaysPins[i]));
    Serial.print("Relay:");
    Serial.println(i);
  }
  Relay::applyToAll(relays, Relay::Status::OFF);
  sensors.begin();
  Serial.println("Temeprature sensor initialized");
  sensors.requestTemperatures();
  hysteresis.insert(sensors.getTempCByIndex(0));
  Serial.println("SETUP:DONE");
};
/**
 *
 */
void loop(void) {
  sensors.requestTemperatures();
  hysteresis.insert(sensors.getTempCByIndex(0));
  Stage::check();
  switch (Stage::value) {
    case Stage::Name::HEATING_METHANOL:
      heating(temperatures[0]);
      break;
    case Stage::Name::METHANOL:
      maintaining(temperatures[0]);
      break;
    case Stage::Name::HEATING_ETHANOL:
      heating(temperatures[1]);
      break;
    case Stage::Name::ETHANOL:
      maintaining(temperatures[1]);
      break;
    case Stage::Name::HEATING_THIRD:
      heating(temperatures[2]);
      break;
    case Stage::Name::THIRD:
      maintaining(temperatures[2]);
      break;

    case Stage::Name::HEATING_FOURTH:
      heating(temperatures[3]);
      break;
    case Stage::Name::FOURTH:
      maintaining(temperatures[3]);
      break;
    default:
      Relay::applyToAll(relays, Relay::Status::OFF);
      break;
  }
  refreshDisplay();
};
