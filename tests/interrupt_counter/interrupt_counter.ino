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
#define INTERRUPT0_PIN 2
#define INTERRUPT1_PIN 3

volatile unsigned long interrupt_counter0;
volatile unsigned long interrupt_counter1;
void countup0(){
  interrupt_counter0 += 1;
}

void countup1(){
  interrupt_counter1 += 1;
}

void setup(){
	Serial.begin(BAUD_RATE);
	pinMode(INTERRUPT0_PIN, INPUT);
	pinMode(INTERRUPT1_PIN, INPUT);
  interrupt_counter0 = 0;
  interrupt_counter1 = 0;
  attachInterrupt(digitalPinToInterrupt(INTERRUPT0_PIN), countup0, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT1_PIN), countup1, RISING);
}

void loop(){
	DEBUG("interrupt_counter0=");
	DEBUG(interrupt_counter0);
	DEBUG(" interrupt_counter1=");
	DEBUG(interrupt_counter1);
	DEBUGLN();
  delay(10);
}
