#include "src/snow-station/SnowStation.h" 
#include <DHT.h>
#include <DFRobot_SHT20.h>
#include <RTClib.h>
#include <LCD5110_Graph.h>

#define BAUD_RATE 500000
#define SD_PIN 53
#define SD_TRANSFER_DATA_LEDPIN 3
#define SD_WRITE_LEDPIN 4
#define SD_TRANSFER_DATA_BUTTONPIN 40
#define DHTTYPE DHT22
#define DHT_PIN 45
#define HC_TRIGGER_PIN 22
#define HC_ECHO_PIN 23

DHT dht_sensor(DHT_PIN, DHT22); // instanciated before SnowStation so we use pointer as construct parameter
DFRobot_SHT20 sht_sensor;
RTC_DS1307 rtc_clock;
LCD5110 screen(8, 9, 10, 12, 11); // 84x48 pixels

SnowStation snow_station(\
	SD_PIN,\
	SD_TRANSFER_DATA_LEDPIN,\
	SD_WRITE_LEDPIN,\
	SD_TRANSFER_DATA_BUTTONPIN,\
	&dht_sensor,\
	&sht_sensor,\
	&rtc_clock,\
	HC_TRIGGER_PIN,\
	HC_ECHO_PIN,\
	&screen
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
