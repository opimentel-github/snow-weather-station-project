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

float get_distance(float v1, float v2){
	float d = abs(v1-v2);
	return d;
}

//############################################################

SnowStation::SnowStation(int _sd_pin, int _sd_transfer_data_ledpin, int _sd_write_ledpin, int _sd_transfer_data_buttonpin, DHT* _dht_sensor, DFRobot_SHT20* _sht_sensor, RTC_DS1307* _rtc_clock, int _hc_trigger_pin, int _hc_echo_pin, LCD5110* _screen){
	sd_pin = _sd_pin;
	sd_transfer_data_ledpin = Ledpin(_sd_transfer_data_ledpin);
	sd_write_ledpin = Ledpin(_sd_write_ledpin);
	sd_transfer_data_buttonpin = Buttonpin(_sd_transfer_data_buttonpin);
	dht_sensor = _dht_sensor;
	sht_sensor = _sht_sensor;
	rtc_clock = _rtc_clock;
	hc_trigger_pin = _hc_trigger_pin;
	hc_echo_pin = _hc_echo_pin;
	screen = _screen;
}

SnowStation::SnowStation(void){
	;
}

void SnowStation::begin(){
	change_state(STATE_IDLE);
	loop_counter = 0;
	saved_records_counter = 0;
	copied_sds_counter = 0;

	begin_ledpins();
	begin_buttonpins();
	begin_screen();
	begin_sd();
	change_state(STATE_SENSING_OK);
}

//############################################################

bool SnowStation::begin_clock(bool uses_compilation_date=false){
	delay(SHORT_DELAY);
	DEBUG("begining clock");
	bool initialized = rtc_clock->begin();
	DEBUGLN();
	if (initialized && uses_compilation_date){
		rtc_clock->adjust(DateTime(F(__DATE__), F(__TIME__)));
		DEBUG("compilation_date="); DEBUGLN(__DATE__);
		DEBUG("compilation_hour="); DEBUGLN(__TIME__);
	}
	return initialized;
}

bool SnowStation::begin_internal_temperature_sensor(){
	delay(SHORT_DELAY);
	DEBUG("begining DHT sensor");
	dht_sensor->begin();
	DEBUGLN();
	return true;
}

bool SnowStation::begin_external_temperature_sensor(){
	delay(SHORT_DELAY);
	DEBUG("begining SHT sensor");
	sht_sensor->initSHT20();
	DEBUGLN();
	return true;
}

bool SnowStation::begin_windrain_sensor(){
	delay(SHORT_DELAY);
	return true;
}

bool SnowStation::begin_snow_distance_sensor(){
	delay(SHORT_DELAY);
	DEBUG("begining HC sensor");
	pinMode(hc_trigger_pin, OUTPUT);
	pinMode(hc_echo_pin, INPUT);
	DEBUGLN();
	return true;
}

//############################################################

bool SnowStation::begin_ledpins(){
	delay(SHORT_DELAY);
	sd_transfer_data_ledpin.begin();
	sd_write_ledpin.begin();
}

bool SnowStation::begin_buttonpins(){
	delay(SHORT_DELAY);
	sd_transfer_data_buttonpin.begin();
}

bool SnowStation::begin_sd(){
	delay(SHORT_DELAY);
	DEBUG("begining SD card");
	pinMode(sd_pin, OUTPUT); digitalWrite(sd_pin, HIGH);
	while(!SD.begin(sd_pin)){
		DEBUG(".");
	}
	DEBUGLN();

	// new file
	delay(SHORT_DELAY);
	update_buffer(); // mainly to update clock
	sprintf(record_filedir, "%s/%04d%02d%02d.txt",\
		SD_ROOTDIR,\
		date_info.year,\
		date_info.month,\
		date_info.day\
		);
	DEBUG("record_filedir="); DEBUGLN(record_filedir);
	if (!SD.exists(record_filedir)){
		SD.mkdir(SD_ROOTDIR);
		File file = SD.open(record_filedir, FILE_WRITE);
		sprintf(sd_buffer_text, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",\
			"SRC",\
			"CSC",\
			"S",\
			"JDATE",\
			"JDAY",\
			"IT",\
			"IH",\
			"ET",\
			"EH",\
			"WS",\
			"WD",\
			"RC",\
			"SD"\
			);
		file.println(sd_buffer_text); // write buffer
		file.close(); // close the file
		DEBUG("sd_buffer_text="); DEBUGLN(sd_buffer_text);
	}
	return true;
}

bool SnowStation::begin_screen(){
	delay(SHORT_DELAY);
	screen->InitLCD(CONTRAST);
	return true;
}

//############################################################

DateInfo SnowStation::get_date_data(){
	delay(SHORT_DELAY);
	DateInfo date_info;
	if (!begin_clock(USES_COMPILATION_DATE)){
		return date_info;
	}

	DateTime rtc_datetime = rtc_clock->now();
	unsigned int year = rtc_datetime.year();
	unsigned int month = rtc_datetime.month();
	unsigned int day = rtc_datetime.day();
	unsigned int new_year;
	unsigned int new_month;
	if(month<=2){
		new_year = year-1;
		new_month = month+12;
	}else{
		new_year = year;
		new_month = month;
	}
	int a = new_year/100;
	int b = 2-a+a/4;
	unsigned long julian_date = (long)(365.25*(new_year+4716))+(int)(30.6001*(new_month+1))+day+b-1524;

	unsigned int hour = rtc_datetime.hour();
	unsigned int minute = rtc_datetime.minute();
	unsigned int second = rtc_datetime.second();
	float julian_day = (float)hour/24.0+(float)minute/1440.0+(float)second/86400.0-0.5;

	date_info.year = year;
	date_info.month = month;
	date_info.day = day;
	date_info.hour = hour;
	date_info.minute = minute;
	date_info.second = second;
	date_info.julian_date = julian_date;
	date_info.julian_day = julian_day;
	return date_info;
}

TempInfo SnowStation::get_internal_temperature_data(){
	delay(SHORT_DELAY);
	TempInfo temp_info;
	if (!begin_internal_temperature_sensor()){
		return temp_info;
	}

	temp_info.humidity = dht_sensor->readHumidity();
	temp_info.temperature = dht_sensor->readTemperature();
	return temp_info;
}

TempInfo SnowStation::get_external_temperature_data(){
	delay(SHORT_DELAY);
	TempInfo temp_info;
	if (!begin_external_temperature_sensor()){
		return temp_info;
	}

	temp_info.humidity = sht_sensor->readHumidity();
	temp_info.temperature = sht_sensor->readTemperature();
	return temp_info;
}

WindrainInfo SnowStation::get_windrain_data(){
	delay(SHORT_DELAY);
	WindrainInfo windrain_info;
	if (!begin_windrain_sensor()){
		return windrain_info;
	}

	Wire.beginTransmission(8);
	Wire.write(0);
	Wire.endTransmission();
	Wire.requestFrom(8, 12);
	float speed;
	float direction;
	float rain_cumulated;
    ((uint8_t*)&speed)[0] = Wire.read();
    ((uint8_t*)&speed)[1] = Wire.read();
    ((uint8_t*)&speed)[2] = Wire.read();
    ((uint8_t*)&speed)[3] = Wire.read();
    ((uint8_t*)&direction)[0] = Wire.read();
    ((uint8_t*)&direction)[1] = Wire.read();
    ((uint8_t*)&direction)[2] = Wire.read();
    ((uint8_t*)&direction)[3] = Wire.read();
    ((uint8_t*)&rain_cumulated)[0] = Wire.read();
    ((uint8_t*)&rain_cumulated)[1] = Wire.read();
    ((uint8_t*)&rain_cumulated)[2] = Wire.read();
    ((uint8_t*)&rain_cumulated)[3] = Wire.read();

	windrain_info.wind_speed = speed;
	windrain_info.wind_direction = direction;
	windrain_info.rain_cumulated = rain_cumulated;
	return windrain_info;
}

SnowInfo SnowStation::get_snow_data(){
	delay(SHORT_DELAY);
	SnowInfo snow_info;
	if (!begin_snow_distance_sensor()){
		return snow_info;
	}

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
	if (duration<=0){
		return snow_info;
	}
	float c; // speed of sound
	float temperature = internal_temp_info.temperature;
	if(USES_FIXED_C==1){
		c = 331.3+0.606*temperature/100; // estimate speed of sound from temperature
	}else{
		c = 10000.0/29.0; // Original value for c in code
	}

	snow_info.distance = microseconds_to_centimeters(duration, c);
	return snow_info;
	// float avg_hc_distance = 0;
	// for(int i=0; i<HC_AVG; i++){
	// 	avg_hc_distance += get_distance_data()/HC_AVG;
	// }
	// snow_distance = SNOW_BASE_DISTANCE-avg_hc_distance;
	// DEBUG("snow_distance="); DEBUGLN(snow_distance);
	// delay(SHORT_DELAY);
}

void SnowStation::update_all_sensor_data(){
	date_info = get_date_data();
	internal_temp_info = get_internal_temperature_data();
	external_temp_info = get_external_temperature_data();
	windrain_info = get_windrain_data();
	snow_info = get_snow_data();
}

//############################################################

void SnowStation::update_screen(){
	screen->clrScr();
	screen->setFont(TinyFont);

	sprintf(screen_buffer_text,
		"%04d/%02d/%02d %02d:%02d:%02d",
		date_info.year, date_info.month, date_info.day, date_info.hour, date_info.minute, date_info.second
		);
	screen->print(screen_buffer_text, 0, SCREEN_DY*0);
	screen->drawLine(0, 6, 84, 6);

	sprintf(screen_buffer_text,
		"IT=%s; ET=%s",
		String(internal_temp_info.temperature, NOF_DECIMALS).c_str(),
		String(external_temp_info.temperature, NOF_DECIMALS).c_str()
		);
	screen->print(screen_buffer_text, 0, SCREEN_DY*1);

	sprintf(screen_buffer_text,
		"IH=%s; EH=%s",
		String(internal_temp_info.humidity, NOF_DECIMALS).c_str(),
		String(external_temp_info.humidity, NOF_DECIMALS).c_str()
		);
	screen->print(screen_buffer_text, 0, SCREEN_DY*2);

	sprintf(screen_buffer_text,
		"WS=%s; WD=%s",
		String(windrain_info.wind_speed, NOF_DECIMALS).c_str(),
		String(windrain_info.wind_direction, NOF_DECIMALS).c_str()
		);
	screen->print(screen_buffer_text, 0, SCREEN_DY*3);

	sprintf(screen_buffer_text,
		"RC=%s; SD=%s",
		String(windrain_info.rain_cumulated, NOF_DECIMALS).c_str(),
		String(snow_info.distance, NOF_DECIMALS).c_str()
		);
	screen->print(screen_buffer_text, 0, SCREEN_DY*4);

	screen->update();
}

void SnowStation::update_buffer(){
	update_all_sensor_data();
	sprintf(sd_buffer_text, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",\
		String(saved_records_counter).c_str(),\
		String(copied_sds_counter).c_str(),\
		String(state).c_str(),\
		String(date_info.julian_date).c_str(),\
		String(date_info.julian_day, 10).c_str(),\
		String(internal_temp_info.temperature, NOF_DECIMALS).c_str(),\
		String(internal_temp_info.humidity, NOF_DECIMALS).c_str(),\
		String(external_temp_info.temperature, NOF_DECIMALS).c_str(),\
		String(external_temp_info.humidity, NOF_DECIMALS).c_str(),\
		String(windrain_info.wind_speed, NOF_DECIMALS).c_str(),\
		String(windrain_info.wind_direction, NOF_DECIMALS).c_str(),\
		String(windrain_info.rain_cumulated, NOF_DECIMALS).c_str(),\
		String(snow_info.distance, NOF_DECIMALS).c_str()\
		);
	DEBUG("sd_buffer_text="); DEBUGLN(sd_buffer_text);
	update_screen();
}

//############################################################

bool SnowStation::save_record(){
	delay(SHORT_DELAY);
	update_buffer();
	File file = SD.open(record_filedir, FILE_WRITE);
	DEBUG("record_filedir="); DEBUGLN(record_filedir);
	if (file){
		file.println(sd_buffer_text); // write buffer
		file.close(); // close the file
		sd_write_ledpin.pulse(2);
	}else{
		change_state(STATE_SD_ERROR);
		return false;
	}
	saved_records_counter += 1;
	return true;
}

bool SnowStation::wait_serial_response(){
	unsigned long k = 0;
	while (true){
		if (Serial3.available()){
			serial_str = Serial3.readStringUntil('\n');
			DEBUGLN(serial_str);
			return true;
		}
		k += 1;
		DEBUGLN(k);
		if (k>100000){
			return false;
		}
	}
}

bool SnowStation::copy_files(){
	File dir;
	File file;
	Serial3.println("--1");
	bool init_success = wait_serial_response();
	if (!init_success){
		return false;
	}
	dir = SD.open("/r/");
	while(true){
		file = dir.openNextFile();
		if(!file){
			file.close(); // just in case
			break;
		}
		String filename = file.name();
		Serial3.println("--o "+filename);
		wait_serial_response();
		while (file.available()){
			String read_buffer = file.readStringUntil('\n');
			sprintf(sd_copy_buffer_text, "--w %s", read_buffer.c_str());
			Serial3.println(sd_copy_buffer_text);
			wait_serial_response();
		}
		file.close();
		Serial3.println("--c "+filename);
		wait_serial_response();
	}
	dir.close();
	Serial3.println("--0");
	wait_serial_response();
	return true;
}

//############################################################

void SnowStation::print_info(){
	DEBUG("state="); DEBUGLN(state);
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
}

unsigned long SnowStation::get_loop_counter(){
	return loop_counter;
}

//############################################################

void SnowStation::loop(){
	add_loop_counter();
	if(state==STATE_SENSING_OK){ // STATE
		if(sd_transfer_data_buttonpin.get_state()){
			change_state(STATE_COPYING_OK);
		}else{
			delay(LOOP_DELAY);
			if(loop_counter>MAX_LOOP_COUNTER){
				bool record_saved = save_record();
				if(record_saved){
					change_state(STATE_SENSING_OK);
				}else{
					change_state(STATE_SD_ERROR);
				}
			}
		}

	}else if(state==STATE_SENSOR_ERROR){ // STATE
		delay(LONG_DELAY);
		change_state(STATE_SENSING_OK);

	}else if(state==STATE_SD_ERROR){ // STATE
		begin_sd();
		change_state(STATE_SENSING_OK);

	}else if(state==STATE_COPYING_OK){ // STATE
		sd_transfer_data_ledpin.high();
		copy_files();
		delay(LONG_DELAY);
		sd_transfer_data_ledpin.low();
		change_state(STATE_COPYING_FINISH);

	}else if(state==STATE_COPYING_FINISH){ // STATE
		if(!sd_transfer_data_buttonpin.get_state()){
			copied_sds_counter += 1;
			change_state(STATE_SENSING_OK);
		}

	}else{
		;
	}
}
