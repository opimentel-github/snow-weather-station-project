/*
*/
#ifndef SnowStation_h
#define SnowStation_h

#include "Arduino.h"
#include <SD.h>
#include "DHT.h"
#include "RTClib.h"
#include <Ledpin.h>
#include <Buttonpin.h>

#define __DEBUG__
#ifdef __DEBUG__
#define DEBUG(...) Serial.print(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define DELIMITER ':'

#define USES_FIXED_C 1
#define SHORT_DELAY 50
#define LONG_DELAY 500

#define SNOW_BASE_DISTANCE 100.0

#define HC_AVG 5
#define SD_ROOTDIR "r"

#define STATE_IDLE -1
#define STATE_SENSING_OK 0
#define STATE_SENSOR_ERROR 1
#define STATE_INTERNAL_SD_ERROR 2
#define STATE_READING_OK 3
#define STATE_READING_FINISH 4
#define STATE_EXTERNAL_SD_ERROR 5

//############################################################

class SnowStation
{
	public:
		SnowStation(int _sd_pin, int _sd_transfer_data_ledpin, int _sd_write_ledpin, int _sd_transfer_data_buttonpin, DHT* _dht_sensor, RTC_DS1307* _rtc_clock, int _hc_trigger_pin, int _hc_echo_pin);
		SnowStation(void); // empty constructor

		// begins
		void begin();
		void begin_ledpins();
		void init_loop();
		void begin_dht();
		void begin_hc();
		void begin_clock();
		void begin_sd();

		// updates
		void update_dht();
		float get_hc();
		void update_hc();
		void update_date();

		// internal sd
		void fill_buffer();
		bool save_record();
		bool copy_files();

		// info
		void print_sd_buffer();
		void print_info();

		// state
		void change_state(int new_state);
		void report_state();
		void reset_loop_counter();

		// loop
		void loop();
	private:
		int sd_pin;
		Ledpin sd_transfer_data_ledpin;
		Ledpin sd_write_ledpin;
		Buttonpin sd_transfer_data_buttonpin;
		DHT* dht_sensor;
		RTC_DS1307* rtc_clock;
		int hc_trigger_pin;
		int hc_echo_pin;

		int state;
		long loop_counter;
		File file;
		long save_record_counter;
		char record_filedir[15];
		char buffer_text[150];
		float internal_humidity;
		float internal_temperature;
		float external_humidity;
		float external_temperature;
		float snow_distance;
		DateTime date;
		long julian_date;
		float julian_day;
};

#endif