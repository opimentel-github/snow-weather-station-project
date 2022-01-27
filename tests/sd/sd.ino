#include <SPI.h>
#include <SD.h>
File myFile;

#define BAUD_RATE 500000
#define SD_PIN 10

void setup() {
// Open serial communications and wait for port to open:
Serial.begin(BAUD_RATE);
while (!Serial) {
	Serial.print("Initializing SD card..."); // wait for serial port to connect. Needed for native USB port only
}
while(!SD.begin(SD_PIN)) {
	Serial.println("initialization sd!");
}
Serial.println("initialization done.");
// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.

myFile = SD.open("test.txt", FILE_WRITE);
// if the file opened okay, write to it:
if (myFile) {
	Serial.print("Writing to test.txt...");
	myFile.println("This is a test file :)");
	myFile.println("testing 1, 2, 3.");
	for (int i = 0; i < 20; i++) {
	myFile.println(i);
	}
	// close the file:
	myFile.close();
	Serial.println("done.");
} else {
	// if the file didn't open, print an error:
	Serial.println("error opening test.txt");
}
}
void loop() {
// nothing happens after setup
}
