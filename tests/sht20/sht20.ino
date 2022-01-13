/*
modified on Apr 10, 2021
Modified by MehranMaleki from Arduino Examples
Home
*/

#include <Wire.h>
#include "DFRobot_SHT20.h"

#define BAUD_RATE 500000
DFRobot_SHT20 sht20;

void setup()
{
    Serial.begin(BAUD_RATE);
    Serial.println("SHT20 Example!");
    sht20.initSHT20();                         // Init SHT20 Sensor
    delay(100);
    sht20.checkSHT20();                        // Check SHT20 Sensor
}

void loop()
{
    float humd = sht20.readHumidity();         // Read Humidity
    float temp = sht20.readTemperature();      // Read Temperature
    Serial.print(" Temperature: ");
    Serial.print(temp, 1);
    Serial.print("C");
    Serial.print("\t Humidity: ");
    Serial.print(humd, 1);
    Serial.println("%");
    delay(1000);
}
