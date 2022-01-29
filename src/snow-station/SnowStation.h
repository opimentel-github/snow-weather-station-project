/*
*/
#ifndef SnowStation_h
#define SnowStation_h

#define __DEBUG__
#ifdef __DEBUG__
#define DEBUG(...) Serial.print(__VA_ARGS__)
#define DEBUGLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#define DEBUGLN(...)
#endif

#include "Arduino.h"
#include <Ledpin.h>
#include <Buttonpin.h>
#include <SD.h>
#include <DHT.h>
#include <DFRobot_SHT20.h>
#include <RTClib.h>
#include <LCD5110_Graph.h>
#include <float.h>

#define DELIMITER ':'
#define USES_FIXED_C 1
#define CONTRAST 70
#define SCREEN_DY 8
#define SHORT_DELAY 10
#define LONG_DELAY 500
#define USES_COMPILATION_DATE false

#define MAX_UNSIGNED_LONG 4294967295

#define NOF_DECIMALS 3
#define HC_AVG 5
#define SD_ROOTDIR "r"
#define LOOP_DELAY 1000
#define MAX_LOOP_COUNTER 10 // 10 10000

#define ANEMOMETER_VOLTAGE 5.0

#define STATE_IDLE -1
#define STATE_SENSING_OK 0
#define STATE_SENSOR_ERROR 1
#define STATE_SD_ERROR 2
#define STATE_COPYING_OK 3
#define STATE_COPYING_FINISH 4

extern unsigned char MediumNumbers[];
extern unsigned char BigNumbers[];
extern unsigned char SmallFont[];
extern unsigned char TinyFont[];
extern int WIND_VANE_RECORD_SECS;
const float v_array[] = {3.84, 1.98, 2.25, 0.41, 0.45, 0.32, 0.9, 0.62, 1.4, 1.19, 3.08, 2.93, 4.62, 4.04, 4.2, 3.43}; // bug: changed 4.78 to 4.2
const float dir_array[] = {0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 315, 337.5};

//############################################################

struct DateInfo{
	unsigned int year = NAN;
	unsigned int month = NAN;
	unsigned int day = NAN;
	unsigned int hour = NAN;
	unsigned int minute = NAN;
	unsigned int second = NAN;
	unsigned long julian_date = NAN;
	float julian_day = NAN;
};

struct TempInfo{
	float temperature = NAN;
	float humidity = NAN;
};

struct WindrainInfo{
	float wind_speed = NAN;
	float wind_direction = NAN;
	float rain_cumulated = NAN;
};

struct SnowInfo{
	float distance = NAN;
};

//############################################################

class SnowStation{
	public:
		SnowStation(int _sd_pin, int _sd_transfer_data_ledpin, int _sd_write_ledpin, int _sd_transfer_data_buttonpin, DHT* _dht_sensor, DFRobot_SHT20* _sht_sensor, RTC_DS1307* _rtc_clock, int _hc_trigger_pin, int _hc_echo_pin, LCD5110* _screen);
		SnowStation(void); // empty constructor
		void begin();

		// begins
		bool begin_clock(bool set_compilation_date=false);
		bool begin_internal_temperature_sensor();
		bool begin_external_temperature_sensor();
		bool begin_windrain_sensor();
		bool begin_snow_distance_sensor();

		bool begin_ledpins();
		bool begin_buttonpins();
		bool begin_screen();
		bool begin_sd();

		DateInfo get_date_data();
		TempInfo get_internal_temperature_data();
		TempInfo get_external_temperature_data();
		WindrainInfo get_windrain_data();
		SnowInfo get_snow_data();
		void update_all_sensor_data();
		
		// updates
		void update_screen();
		void update_buffer();

		// internal sd
		bool save_record();
		bool wait_serial_response();
		bool copy_files();

		// info
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
		DFRobot_SHT20* sht_sensor;
		RTC_DS1307* rtc_clock;
		int hc_trigger_pin;
		int hc_echo_pin;
		LCD5110* screen;

		int state;
		unsigned long loop_counter;
		unsigned long saved_records_counter;
		unsigned long copied_sds_counter;
		char record_filedir[20];
		char sd_buffer_text[300];
		char sd_copy_buffer_text[300];
		char screen_buffer_text[100];
		String serial_str;

		DateInfo date_info;
		TempInfo internal_temp_info;
		TempInfo external_temp_info;
		WindrainInfo windrain_info;
		SnowInfo snow_info;
};

#endif