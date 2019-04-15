/**
  \file SEF.c
  Implementation of the SEF library.
*/

#include "Arduino.h"
#include "SEF.h"

#define _hcTrigPin 8
#define _hcEchoPin 9

unsigned int threshold;

/* ------ INTERNAL UTILITY FUNCTIONS ------*/

/**
  Internal utility function for a single read from the HC-SR04 sensor.
  \return detected distance in cm
*/
unsigned int hc_read() {
  unsigned long duration;
  unsigned int distance;

  // Clearing trigPin
  digitalWrite(_hcTrigPin, LOW);
  delayMicroseconds(2);

  // Sending a pulse for 10 µs
  digitalWrite(_hcTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_hcTrigPin,LOW);

  // Reading the echo pin for the return of the pulse.
  //This section is considered critical (interrupts disabled)
  noInterrupts();
  /* Using the 3 args version of pulseIn because, apparently, the overloaded 2-args version is only supported in C++.
  I'm giving as 3rd argument 1000000L, which is how the overloading is done in the standard library found in C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h*/
  duration = pulseIn(_hcEchoPin, HIGH);
  interrupts();
  // Calculating the distance
  distance = duration*0.034/2;

  return distance;
}


/* ------ LIBRARY HEADER FUNCTIONS ------*/
void initSEF() {
  // HC-SR04 setup
  pinMode(_hcTrigPin, OUTPUT); // Trigger pin as an output
  pinMode(_hcEchoPin, INPUT); // Echo pin as an input
}

unsigned int readDistance() {
  unsigned int distance;

  distance = hc_read();

  return distance;
}
