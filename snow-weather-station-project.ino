#include "src/snow-station/SnowStation.h" 
#include <DHT.h>
#include <DFRobot_SHT20.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BAUD_RATE 500000
#define WIND_VANE_RECORD_SECS 3

#define SD_PIN 53
#define SD_TRANSFER_DATA_LEDPIN 38
#define SD_WRITE_LEDPIN 39
#define SD_TRANSFER_DATA_BUTTONPIN 40
#define DHTTYPE DHT22
#define DHT_PIN 45
#define HC_TRIGGER_PIN 34
#define HC_ECHO_PIN 35
#define ANEMOMETER_PIN 2

DHT dht_sensor(DHT_PIN, DHT22); // instanciated before SnowStation so we use pointer as construct parameter
DFRobot_SHT20 sht_sensor;
RTC_DS1307 rtc_clock;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SnowStation snow_station(
	SD_PIN,
	SD_TRANSFER_DATA_LEDPIN,
	SD_WRITE_LEDPIN,
	SD_TRANSFER_DATA_BUTTONPIN,
	&dht_sensor,
	&sht_sensor,
	&rtc_clock,
	HC_TRIGGER_PIN,
	HC_ECHO_PIN,
	&display
	);

//############################################################

void setup(){
	digitalWrite(SDA, LOW); // disables internal pull-ups
	digitalWrite(SCL, LOW); // disables internal pull-ups
	Serial.begin(BAUD_RATE);
	Serial3.begin(BAUD_RATE);
	snow_station.begin();
}

//############################################################

void loop(){
	snow_station.loop();
	// snow_station.print_info();
}
