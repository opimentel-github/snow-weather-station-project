/*
*/
#ifndef SnowStation_h
#define SnowStation_h

#include "Arduino.h"
#include <Ledpin.h>
#include <Buttonpin.h>
#include <SD.h>
#include "DHT.h"
#include "RTClib.h"
#include <LCD5110_Graph.h>

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
#define CONTRAST 70
#define SCREEN_DY 9

#define MAX_UNSIGNED_LONG 4294967295

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

extern unsigned char MediumNumbers[];
extern unsigned char BigNumbers[];
extern unsigned char SmallFont[];
extern unsigned char TinyFont[];

//############################################################

class SnowStation
{
	public:
		SnowStation(int _sd_pin, int _sd_transfer_data_ledpin, int _sd_write_ledpin, int _sd_transfer_data_buttonpin, DHT* _dht_sensor, RTC_DS1307* _rtc_clock, int _hc_trigger_pin, int _hc_echo_pin, LCD5110* _screen);
		SnowStation(void); // empty constructor

		// begins
		void begin();
		void begin_ledpins();
		void init_loop();
		void begin_dht();
		void begin_hc();
		void begin_clock();
		void begin_sd();
		void begin_screen();

		// updates
		void update_dht();
		float get_hc();
		void update_hc();
		void update_date();
		void update_screen_line(int x, int y, String key, String value);
		void update_screen();

		// internal sd
		void fill_buffer();
		bool save_record();
		bool copy_files();

		// info
		void print_sd_buffer();
		void print_info();

		// state
		void change_state(int new_state, unsigned long new_loop_counter_value=0);
		void report_state();
		void reset_loop_counter(unsigned long new_loop_counter_value=0);
		void add_loop_counter();
		unsigned long get_loop_counter();

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
		LCD5110* screen;

		int state;
		unsigned long loop_counter;
		File file;
		unsigned long save_record_counter;
		char record_filedir[15];
		char sd_buffer_text[200];
		char screen_buffer_text[100];
		float internal_humidity;
		float internal_temperature;
		float external_humidity;
		float external_temperature;
		float snow_distance;
		DateTime date;
		unsigned long julian_date;
		float julian_day;
};

#endif