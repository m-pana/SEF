/**
  \file SEF.c
  Implementation of the SEF library.
*/

#include "Arduino.h"
#include "SEF.h"
#include "LowPower.h"

/* ------ PIN LAYOUT ------ */
#define _vibrationPin 7
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
  // Vibraing motor setup
  pinMode(_vibrationPin, OUTPUT);

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

void userWarning(bool detectionResult) {
  // --- Implementation Option A (as described in documentation)
  if (detectionResult == true) {
    // if found, vibrate for 250ms
    digitalWrite(_vibrationPin, HIGH);
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    digitalWrite(_vibrationPin, LOW);
  } else {
    // if not, just sleep for 250ms
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
  } 
  
  void sendWarning()
{  
   int low = 2; //something we will have to define ourselves
   int sensorValue = analogRead(A0); //read the A0 pin value
   //the analog reading goes from 0 to 1023 and we need to convert it as a voltage from 0 to 5V
   float voltage = sensorValue * (5.00 / 1023.00) * 2; //convert the value to a true voltage. 

   if (voltage <= low)
   {
        digitalWrite(_vibrationPin, HIGH);
        LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
        digitalWrite(_vibrationPin, LOW);
   }  

   
} 

void sensorActive()
{
  //test if a sensor is active by trying to detect if it detects something  
   // Sending a pulse for 10 µs
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  // Reading the echo pin for the return of the pulse
  duration = pulseIn(echoPin, HIGH);

  
  //pulseIn() method returns the length of the pulse (in microseconds) or 0 if no pulse started before the timeout (unsigned long) 
  if (duration == 0)
  {
     //decide of a signal we could use 
     //here the vibration 
      digitalWrite(_vibrationPin, HIGH);
      LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
      digitalWrite(_vibrationPin, LOW);
  }
}

  /*
  // --- Implementation Option B (as described in documentation)
  if (detectionResult == false) {
    // if not found, sleep for 280ms
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
  } else if (below 1/3 of threshold) {
    // Vibrate for 280ms
    digitalWrite(_vibrationPin, HIGH);
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
    digitalWrite(_vibrationPin, LOW);
  } else if (below 2/3 of threshold) {
    // Alternate 120ms of vibration and 60ms of sleep
    digitalWrite(_vibrationPin, HIGH);
    LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);

    digitalWrite(_vibrationPin, LOW);
    LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);

    digitalWrite(_vibrationPin, HIGH);
    LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);

    digitalWrite(_vibrationPin, LOW);
  } else {
    // Alternate 60ms of vibration and 60ms of sleep
    for (int i = 0; i < 2; i++) {
      digitalWrite(_vibrationPin, HIGH);
      LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
      digitalWrite(_vibrationPin, LOW);
      LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
    }

    digitalWrite(_vibrationPin, HIGH);
    LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
  }
  */

  /*
  // --- Implementation Option C (as described in documentation)
  // turn off vibration anyway and sleep for 60ms
  digitalWrite(_vibrationPin, LOW);
  LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
  // if found, then turn vibration on; otherwise do nothing (vibration stays off)
  if (detectionResult == true) {
    digitalWrite(_vibrationPin, HIGH);
  }
  */

}

int getThreshold() {
  return threshold;
}
