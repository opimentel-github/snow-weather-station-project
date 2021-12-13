#include <SnowStation.h>
#include <Wire.h>
#include "DHT.h"
#include "RTClib.h"

#define INTERNAL_SD_ALERT_PIN 3
#define EXTERNAL_SD_ALERT_PIN 4
#define INTERNAL_SD_CD_PIN 10
#define DHTTYPE DHT22
#define DHT_PIN 2
#define HC_TRIGGER_PIN 6 
#define HC_ECHO_PIN 5

float base_distance = 100.0;

DHT dht_sensor(DHT_PIN, DHT22); // instanciated before SnowStation so we use pointer as construct parameter
RTC_DS1307 rtc_clock;
SnowStation snow_station(\
	INTERNAL_SD_ALERT_PIN,\
	EXTERNAL_SD_ALERT_PIN,\
	INTERNAL_SD_CD_PIN,\
	&dht_sensor,\
	&rtc_clock,\
	HC_TRIGGER_PIN,\
	HC_ECHO_PIN,\
	base_distance\
	);

//############################################################

void setup(){
	Serial.begin(9600);
	snow_station.begin();
	pinMode(7, INPUT);
}

//############################################################

void loop(){
	bool button_state = digitalRead(7);
	snow_station.loop(button_state);
	snow_station.print_info();
}
