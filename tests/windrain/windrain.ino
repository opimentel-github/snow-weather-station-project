#include <float.h>
#include <Wire.h>

#define __DEBUG__
#ifdef __DEBUG__
#define DEBUG(...) Serial.print(__VA_ARGS__)
#define DEBUGLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#define DEBUGLN(...)
#endif

#define BAUD_RATE 500000
#define ANEMOMETER_PIN 2
#define WIND_VANE_PIN A3
#define RAIN_GAUGE_PIN 3
#define INTERRUPT_DELAY_SECS 3.0
#define ANEMOMETER_VOLTAGE 5.0

volatile unsigned long interrupt_counter0;
volatile unsigned long interrupt_counter1;
const float v_array[] = {3.84, 1.98, 2.25, 0.41, 0.45, 0.32, 0.9, 0.62, 1.4, 1.19, 3.08, 2.93, 4.62, 4.04, 4.2, 3.43}; // bug: changed 4.78 to 4.2
const float dir_array[] = {0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 315, 337.5};
float wind_speed = NAN;
float wind_dir = NAN;
float rain_freq = NAN;

float get_distance(float v1, float v2){
	float d = abs(v1-v2);
	return d;
}

float get_wind_direction(unsigned int analog_readed){
	float v_readed = (float)analog_readed/1023.0*ANEMOMETER_VOLTAGE;
	float min_distance = FLT_MAX;
	float v;
	float dir;
	float distance;
	float selected_dir;
	for (int k=0; k<(sizeof(v_array) / sizeof(v_array[0])); k++){
		v = v_array[k];
		dir = dir_array[k];
		distance = get_distance(v_readed, v);
		if (distance<min_distance){
			selected_dir = dir;
			min_distance = distance;
		}
	}
	return selected_dir;
}

void countup0(){
  interrupt_counter0 += 1;
}

void countup1(){
  interrupt_counter1 += 1;
}

void receiveEvent() {
  while (0 < Wire.available()) {
    byte x = Wire.read();
  }
}

void requestEvent() {
   byte response[12] = {
      ((uint8_t*)&wind_speed)[0],
      ((uint8_t*)&wind_speed)[1],
      ((uint8_t*)&wind_speed)[2],
      ((uint8_t*)&wind_speed)[3],
      ((uint8_t*)&wind_dir)[0],
      ((uint8_t*)&wind_dir)[1],
      ((uint8_t*)&wind_dir)[2],
      ((uint8_t*)&wind_dir)[3],
      ((uint8_t*)&rain_freq)[0],
      ((uint8_t*)&rain_freq)[1],
      ((uint8_t*)&rain_freq)[2],
      ((uint8_t*)&rain_freq)[3],
   };
  Wire.write(response, sizeof(response));
}

void setup(){
	Wire.begin(8); // join i2c bus with address #8
  Wire.onRequest(requestEvent); 
  Wire.onReceive(receiveEvent);
	Serial.begin(BAUD_RATE);
	pinMode(ANEMOMETER_PIN, INPUT);
	pinMode(WIND_VANE_PIN, INPUT);
	pinMode(RAIN_GAUGE_PIN, INPUT);
}

void loop(){
	// rain level and wind speed
  interrupt_counter0 = 0;
  interrupt_counter1 = 0;
  attachInterrupt(digitalPinToInterrupt(RAIN_GAUGE_PIN), countup0, RISING);
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN), countup1, RISING);
  delay(1000*INTERRUPT_DELAY_SECS);
  detachInterrupt(digitalPinToInterrupt(RAIN_GAUGE_PIN));
  detachInterrupt(digitalPinToInterrupt(ANEMOMETER_PIN));
  rain_freq = (float)interrupt_counter0/((float)INTERRUPT_DELAY_SECS);
  wind_speed = (float)interrupt_counter1/((float)INTERRUPT_DELAY_SECS*2.4);
	wind_dir = get_wind_direction(analogRead(WIND_VANE_PIN));
	DEBUG("wind_speed=");
	DEBUG(wind_speed);
	DEBUG(" wind_dir=");
	DEBUG(wind_dir);
	DEBUG(" rain_freq=");
	DEBUG(rain_freq);
	DEBUGLN();
}
