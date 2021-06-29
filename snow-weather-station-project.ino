#include <SnowStation.h>
#include "DHT.h"

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHT_PIN 5
#define STATUS_PIN 4

String RECORD_FILEDIR = "record.txt"; // no longer than 8 characters

DHT dht_sensor(DHT_PIN, DHT22);
SnowStation snow_station(&dht_sensor, STATUS_PIN, RECORD_FILEDIR);

//############################################################

void setup(){
	Serial.begin(9600);
	// while (!Serial){;} // wait for serial port to connect. Needed for native USB port only
	Serial.println("init");
	snow_station.begin();
}

//############################################################

void loop(){
	delay(1000);
	Serial.println("in loop");
	snow_station.update_dht();
	snow_station.blink_status();
	snow_station.fill_buffer();
	snow_station.print_buffer();
	snow_station.save_record();
	snow_station.end_loop();
}
