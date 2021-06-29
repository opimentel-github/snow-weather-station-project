#include "DHT.h"
#include <SPI.h>
#include <SD.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

const int PIN_DHT = 5;
const int PIN_SUCCESS_SD = 4;
String RECORD_FILEDIR = "buffer.txt"; // no longer than 8 characters

char buffer_text[50];
File my_file;
long alive_counter = 0;

//############################################################

DHT dht(PIN_DHT, DHTTYPE);
void setup() {
	Serial.begin(9600);
	// while (!Serial){;} // wait for serial port to connect. Needed for native USB port only
	
	pinMode(PIN_SUCCESS_SD, OUTPUT);

	delay(100);
	Serial.println("DHTxx test!");
	dht.begin();

	delay(100);
	Serial.print("Initializing SD card...");
	if (!SD.begin(7)) {
		Serial.println("initialization failed!");
		while (1);
	}
	Serial.println("initialization done.");

	delay(100);
	my_file = SD.open(RECORD_FILEDIR.c_str(), FILE_WRITE);
	// if the file opened okay, write to it:
	if (my_file) {
		my_file.println("alive_counter, humidity, temperature");
		my_file.close(); // close the file
	}
	else {
		Serial.print("error");
		// if the file didn't open, print an error:
		//printf("error opening %s", RECORD_FILEDIR.c_str());
	}
}

//############################################################

void loop() {
	delay(5000);
	int wrong_data = 0;
	float h = dht.readHumidity();
	float t = dht.readTemperature();
	if (isnan(h) || isnan(t)) {
		// Serial.println("Failed to read from DHT sensor!");
		wrong_data = 1;
	}

	my_file = SD.open(RECORD_FILEDIR.c_str(), FILE_WRITE);
	// if the file opened okay, write to it:
	if (my_file) {
		sprintf(buffer_text, "%s, %s, %s", String(alive_counter).c_str(), String(h).c_str(), String(t).c_str());
		Serial.println(buffer_text);
		my_file.println(buffer_text);
		my_file.close(); // close the file
		digitalWrite(PIN_SUCCESS_SD, 1);
	}
	else {
		Serial.print("error");
		//printf("error opening %s", RECORD_FILEDIR.c_str());
		digitalWrite(PIN_SUCCESS_SD, 0);
	}
	alive_counter += 1;
}
