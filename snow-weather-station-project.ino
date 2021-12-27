#include "src/snow-station/SnowStation.h" 
#include "DHT.h"
#include "RTClib.h"

#define BAUD_RATE 500000
#define SD_PIN 53
#define SD_TRANSFER_DATA_LEDPIN 3
#define SD_WRITE_LEDPIN 4
#define SD_TRANSFER_DATA_BUTTONPIN 40
#define DHTTYPE DHT22
#define DHT_PIN 2
#define HC_TRIGGER_PIN 6
#define HC_ECHO_PIN 5

DHT dht_sensor(DHT_PIN, DHT22); // instanciated before SnowStation so we use pointer as construct parameter
RTC_DS1307 rtc_clock;
SnowStation snow_station(\
	SD_PIN,\
	SD_TRANSFER_DATA_LEDPIN,\
	SD_WRITE_LEDPIN,\
	SD_TRANSFER_DATA_BUTTONPIN,\
	&dht_sensor,\
	&rtc_clock,\
	HC_TRIGGER_PIN,\
	HC_ECHO_PIN\
	);

//############################################################

void setup(){
	Serial.begin(BAUD_RATE);
	Serial3.begin(BAUD_RATE);
	snow_station.begin();
}

//############################################################

void loop(){
	snow_station.loop();
	// snow_station.print_info();
}
