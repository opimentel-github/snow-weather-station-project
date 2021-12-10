#include <SnowStation.h>
#include <Wire.h>
#include "DHT.h"
#include "RTClib.h"

#define DHTTYPE DHT22
#define DHT_PIN 2
#define SD_CD_PIN 10
#define STATUS_PIN 3
#define ERROR_PIN 4

DHT dht_sensor(DHT_PIN, DHT22); // instanciated before SnowStation so we use pointer as construct parameter
RTC_DS1307 rtc_clock;
SnowStation snow_station(\
	SD_CD_PIN,\
	&dht_sensor,\
	&rtc_clock,\
	STATUS_PIN,\
	ERROR_PIN\
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
