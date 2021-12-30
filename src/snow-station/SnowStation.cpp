/*
*/
#include "Arduino.h"
#include "SnowStation.h"

//############################################################

float microseconds_to_centimeters(long microseconds, float c) {
	// The speed of sound is 340 m/s or 29 microseconds per centimeter.
	// actually 29 microsec/cm = 10000/29 = 344.8 m/s, ie 22.3 deg C
	// The ping travels out and back, so to find the distance of the
	// object we take half of the distance travelled.
	return microseconds*c/20000.0;
}

//############################################################

SnowStation::SnowStation(int _sd_pin, int _sd_transfer_data_ledpin, int _sd_write_ledpin, int _sd_transfer_data_buttonpin, DHT* _dht_sensor, RTC_DS1307* _rtc_clock, int _hc_trigger_pin, int _hc_echo_pin, LCD5110* _screen){
	sd_pin = _sd_pin;
	sd_transfer_data_ledpin = Ledpin(_sd_transfer_data_ledpin);
	sd_write_ledpin = Ledpin(_sd_write_ledpin);
	sd_transfer_data_buttonpin = Buttonpin(_sd_transfer_data_buttonpin);
	dht_sensor = _dht_sensor;
	rtc_clock = _rtc_clock;
	hc_trigger_pin = _hc_trigger_pin;
	hc_echo_pin = _hc_echo_pin;
	screen = _screen;
}

SnowStation::SnowStation(void){}

//############################################################

void SnowStation::begin(){
	change_state(STATE_IDLE);
	loop_counter = 0;
	save_record_counter = 0;
	begin_ledpins();
	sd_transfer_data_buttonpin.begin();
	
	internal_humidity = NAN;
	internal_temperature = NAN;
	external_humidity = NAN;
	external_temperature = NAN;

	begin_dht();
	begin_hc();
	begin_clock();
	begin_sd();
	begin_screen();

	change_state(STATE_SENSING_OK, MAX_UNSIGNED_LONG-1);
	
}

void SnowStation::begin_screen(){
	screen->InitLCD(CONTRAST);
}

void SnowStation::begin_ledpins(){
	sd_transfer_data_ledpin.begin();
	sd_write_ledpin.begin();
}

void SnowStation::begin_dht(){
	DEBUG("begining DHT sensor");
	dht_sensor->begin();
	delay(SHORT_DELAY);
}

void SnowStation::begin_hc(){
	DEBUG("begining HC sensor");
	pinMode(hc_trigger_pin, OUTPUT);
	pinMode(hc_echo_pin, INPUT);
	delay(SHORT_DELAY);
}

void SnowStation::begin_clock(){
	DEBUG("begining clock");
	rtc_clock->begin();
	// rtc_clock->adjust(DateTime(F(__DATE__), F(__TIME__)));
	// Serial.print("compilation_date="); Serial.println(__DATE__);
	// Serial.print("compilation_hour="); Serial.println(__TIME__);
	delay(SHORT_DELAY);
}

void SnowStation::begin_sd(){
	DEBUG("begining SD card");
	pinMode(sd_pin, OUTPUT); digitalWrite(sd_pin, HIGH);
	while(!SD.begin(sd_pin)){
		DEBUG(".");
	}
	DEBUG('\n');

	// file
	sprintf(record_filedir, "%s/%04d%02d%02d.txt",\
		SD_ROOTDIR,\
		date.year(),\
		date.month(),\
		date.day()\
		);
	Serial.print("record_filedir="); Serial.println(record_filedir);
	if (!SD.exists(record_filedir)){
		SD.mkdir(SD_ROOTDIR);
		file = SD.open(record_filedir, FILE_WRITE);
		sprintf(sd_buffer_text, "%s, %s, %s, %s, %s, %s, %s",\
			"save_record_counter",\
			"state",\
			"julian_date",\
			"julian_day",\
			"internal_temperature",\
			"internal_humidity",\
			"snow_distance"\
			);
		file.println(sd_buffer_text); // write buffer
		file.close(); // close the file
	}
}

//############################################################

void SnowStation::update_dht(){
	internal_humidity = dht_sensor->readHumidity();
	internal_temperature = dht_sensor->readTemperature();
	if (isnan(internal_humidity) || isnan(internal_temperature)){
		Serial.println("DHT sensor error");
	}
	delay(SHORT_DELAY);
}

float SnowStation::get_hc(){
	// The sensor is triggered by a HIGH pulse of 10 or more microseconds.
	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	digitalWrite(hc_trigger_pin, LOW);
	delayMicroseconds(2);
	digitalWrite(hc_trigger_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(hc_trigger_pin, LOW);

	// Read the signal from the sensor: a HIGH pulse whose
	// duration is the time (in microseconds) from the sending
	// of the ping to the reception of its echo off of an object.
	long duration = pulseIn(hc_echo_pin, HIGH);

	// estimate speed of sound from temperature:
	float c; // speed of sound
	float temperature = internal_temperature;
	if(USES_FIXED_C==1){
		c = 331.3+0.606*temperature/100;
	}else{
		c = 10000.0/29.0; // Original value for c in code
	}
	float hc_distance = microseconds_to_centimeters(duration, c);
	Serial.print("hc_distance="); Serial.println(hc_distance);
	delay(SHORT_DELAY);
	return hc_distance;
}

void SnowStation::update_hc(){
	float avg_hc_distance = 0;
	for(int i=0; i<HC_AVG; i++){
		avg_hc_distance += get_hc()/HC_AVG;
	}
	snow_distance = SNOW_BASE_DISTANCE-avg_hc_distance;
	Serial.print("snow_distance="); Serial.println(snow_distance);
	delay(SHORT_DELAY);
}

void SnowStation::update_date(){
	date = rtc_clock->now();
	int year = date.year();
	int month = date.month();
	int day = date.day();
	if(month<=2){
		year -= 1;
		month += 12;
	}
	int a = year/100;
	int b = 2-a+a/4;
	julian_date = (long)(365.25*(year+4716))+(int)(30.6001*(month+1))+day+b-1524;

	float h = date.hour();
	float m = date.minute();
	float s = date.second();
	julian_day = h/24.0+m/1440.0+s/86400.0-0.5;
	delay(SHORT_DELAY);
}

void SnowStation::update_screen_line(int x, int y, String key, String value){
	sprintf(screen_buffer_text, "%s=%s", key.c_str(), value.c_str());
	screen->print(screen_buffer_text, x, y);
}

void SnowStation::update_screen(){
	screen->clrScr();
	screen->setFont(TinyFont);
	// screen->print("last saved", 0, SCREEN_DY*0);
	screen->print(String(date.year())+"/"+String(date.month())+"/"+String(date.day())+" "+String(date.hour())+":"+String(date.minute())+":"+String(date.second()), 0, SCREEN_DY*0);
	screen->drawLine(0, 6, 84, 6);
	update_screen_line(0, SCREEN_DY*3, "temp", String(internal_temperature));
	update_screen_line(0, SCREEN_DY*4, "dist", String(snow_distance));
	screen->update();
}

//############################################################

void SnowStation::fill_buffer(){
	sprintf(sd_buffer_text, "%s, %s, %s, %s, %s, %s, %s",\
		String(save_record_counter).c_str(),\
		String(state).c_str(),\
		String(julian_date).c_str(),\
		String(julian_day, 10).c_str(),\
		String(internal_temperature).c_str(),\
		String(internal_humidity).c_str(),\
		String(snow_distance).c_str()\
		);
	update_screen();
}

bool SnowStation::save_record(){
	bool success = false;
	update_date();
	fill_buffer();
	file = SD.open(record_filedir, FILE_WRITE);
	Serial.print("record_filedir=");
	Serial.println(record_filedir);
	if (file){
		file.println(sd_buffer_text); // write buffer
		file.close(); // close the file
		sd_write_ledpin.pulse(2);
		success = true;
	}else{
		change_state(STATE_INTERNAL_SD_ERROR);
	}
	save_record_counter += 1;
	delay(SHORT_DELAY);
	return success;
}

bool SnowStation::copy_files(){
	bool success = false;
	File dir;
	File file;
	String serial_str;
	dir = SD.open("/r/");
	while(true){
		file = dir.openNextFile();
		if(!file){
			file.close();
			break;
		}
		String filename = file.name();

		DEBUG("removing file...");
		Serial3.print("--o ");
		Serial3.println(filename);
		while (true){
			if (Serial3.available()){
			serial_str = Serial3.readStringUntil('\n');
			Serial.println(serial_str);
			break;
			// if (serial_str=="1"){
			// 	break;
			// }
			}
		}
		while (file.available()){
					// Serial.println("sending buffer...");
					Serial3.print("--w ");
					Serial3.print(filename);
					Serial3.print(":");
					String read_buffer = file.readStringUntil('\n');
					Serial3.println(read_buffer);
					while (true){
						if (Serial3.available()){
						serial_str = Serial3.readStringUntil('\n');
						Serial.println(serial_str);
						break;
						// if (serial_str=='1'){
						// 	break;
						// }
						}
					}
				}
		file.close();
		dir.close();
		Serial3.print("--c ");
		Serial3.println(filename);
		while (true){
			if (Serial3.available()){
			serial_str = Serial3.readStringUntil('\n');
			Serial.println(serial_str);
			break;
			// if (serial_str=="1"){
			// 	break;
			// }
			}
		}
	}
	Serial3.println("--x");
	while (true){
		if (Serial3.available()){
		serial_str = Serial3.readStringUntil('\n');
		Serial.println(serial_str);
		break;
		// if (serial_str=="1"){
		// 	break;
		// }
		}
	}
	return success;
}

//############################################################

void SnowStation::print_sd_buffer(){
	Serial.print("sd_buffer_text="); Serial.println(sd_buffer_text);
}

void SnowStation::print_info(){
	Serial.print("state="); Serial.println(state);
	print_sd_buffer();
}

//############################################################

void SnowStation::change_state(int new_state, unsigned long new_loop_counter_value=0){
	state = new_state;
	report_state();
	reset_loop_counter(new_loop_counter_value);
}

void SnowStation::report_state(){
	print_info();
}

void SnowStation::reset_loop_counter(unsigned long new_loop_counter_value=0){
	loop_counter = new_loop_counter_value;
}

void SnowStation::add_loop_counter(){
	loop_counter += 1;
	delay(10);
}

unsigned long SnowStation::get_loop_counter(){
	return loop_counter;
}

//############################################################

void SnowStation::loop(){
	add_loop_counter();
	if(state==STATE_SENSING_OK){ // STATE
		if(sd_transfer_data_buttonpin.get_state()){
			change_state(STATE_READING_OK);
		}else{
			if(loop_counter>1000){
				update_dht();
				update_hc();
				if(save_record()){
					change_state(STATE_SENSING_OK);
				}else{
					change_state(STATE_INTERNAL_SD_ERROR);
				}
			}
		}

	}else if(state==STATE_SENSOR_ERROR){ // STATE
		delay(LONG_DELAY);
		change_state(STATE_SENSING_OK);

	}else if(state==STATE_INTERNAL_SD_ERROR){ // STATE
		begin_sd();
		change_state(STATE_SENSING_OK);

	}else if(state==STATE_READING_OK){ // STATE
		sd_transfer_data_ledpin.high();
		copy_files();
		delay(LONG_DELAY);
		sd_transfer_data_ledpin.low();
		change_state(STATE_READING_FINISH);

	}else if(state==STATE_READING_FINISH){ // STATE
		if(!sd_transfer_data_buttonpin.get_state()){
			change_state(STATE_SENSING_OK);
		}

	}else if(state==STATE_EXTERNAL_SD_ERROR){ // STATE
		begin_sd();
		change_state(STATE_SENSING_OK);

	}else{
		;
	}
}
