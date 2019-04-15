/**
  \file SEF.c
  Implementation of the SEF library.
*/

#include "Arduino.h"
#include "SEF.h"
#include "LowPower.h"

#define _hcTrigPin 8
#define _hcEchoPin 9

unsigned int threshold = 150;

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
  return hc_read();
}

bool obstacleDetected() {
  // Does 3 readings. Power-down mode is entered in between each of them for 30ms.
  // An obstacle is actually considered to be present of at least 2 of the 3 readings are positive
  int positives = 0;

  for (int i = 0; i < 3; i++) {
    if (hc_read() <= threshold) {
      positives++;
    }

    // Don't sleep on last iteration
    if (i != 2) {
      LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
    }
  }

  if (positives >= 2) {
    return true;
  } else {
    return false;
  }
}


int getThreshold() {
  return threshold;
}
