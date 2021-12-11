#include <SnowStation.h>
#include <Wire.h>
#include "DHT.h"
#include "RTClib.h"

#define STATUS_PIN 3
#define ERROR_PIN 4
#define DHTTYPE DHT22
#define DHT_PIN 2
#define SD_CD_PIN 10
#define HC_TRIGGER_PIN 6 
#define HC_ECHO_PIN 5

float base_distance = 100.0;

DHT dht_sensor(DHT_PIN, DHT22); // instanciated before SnowStation so we use pointer as construct parameter
RTC_DS1307 rtc_clock;
SnowStation snow_station(\
	STATUS_PIN,\
	ERROR_PIN,\
	SD_CD_PIN,\
	&dht_sensor,\
	&rtc_clock,\
	HC_TRIGGER_PIN,\
	HC_ECHO_PIN,\
	base_distance\
	);

//############################################################

void setup(){
	Serial.begin(9600);
	// while (!Serial){;} // wait for serial port to connect. Needed for native USB port only
	Serial.println("init");
	snow_station.begin();
}

//############################################################

void loop(){
	snow_station.init_loop();
	snow_station.update_all_sensors();
	snow_station.save_record();
	snow_station.print_buffer();
	snow_station.end_loop();
	delay(5000);
}
