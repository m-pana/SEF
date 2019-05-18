/**
  \file SEF.c
  Implementation of the SEF library.
*/

#include "Arduino.h"
#include "SEF.h"
#include "LowPower.h"

#define DEBUG

/* ------ PIN LAYOUT ------ */
#define _buzzerPin 6

#define _vibrationPin 7

#define _hcEchoPin 13
#define _hcTrigPin 12
#define _hcVccPin 11

#define _numReadings 3
#define _minPositives 2

#define _modeSwitch 2 // TBD, switch to trigger the interrupt

#define _malfunctioningFrequency 3500
#define _batteryFrequency 700

volatile unsigned int threshold = 150;
volatile int mode; // Value for the interrupt (indoor or outdoor)



/* ------ INTERNAL UTILITY FUNCTIONS ------*/

/* --- INIT FUNCTIONS --- */
/**
  Internal utility function for initializing the vibration
*/
void initVibration() {
  // Vibraing motor setup
  pinMode(_vibrationPin, OUTPUT);
}

/**
  Internal utility function for initializing the HC-SR04 proximity sensor
*/
void initHCSR04() {
  // HC-SR04 setup
  pinMode(_hcTrigPin, OUTPUT); // Trigger pin as an output
  pinMode(_hcEchoPin, INPUT); // Echo pin as an input
  pinMode(_hcVccPin, OUTPUT);
  digitalWrite(_hcVccPin, HIGH); // Needs to be set permanently to HIGH to feed the HCSR04 (this is to plug the sensor directly into the pins of the board)
}

/**
  Internal utility function for initializing the buzzer
*/
void initBuzzer() {
  // Buzzer setup
  pinMode(_buzzerPin, OUTPUT);
}

/**
  Internal utility function for initializing the switch
*/
void initSwitch(){
  pinMode(_modeSwitch, INPUT_PULLUP); // Setting up the pullup
  mode = (digitalRead(_modeSwitch) == HIGH ? OUTDOOR : INDOOR);
  setMode(mode);

  //Setting up the ISR to trigger when a change happens in _modeSwitch pin
  attachInterrupt(digitalPinToInterrupt(_modeSwitch), toggleMode, CHANGE);
}

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
  noInterrupts();
  digitalWrite(_hcTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_hcTrigPin,LOW);

  // Reading the echo pin for the return of the pulse.
  //This section is considered critical (interrupts disabled)
  duration = pulseIn(_hcEchoPin, HIGH);
  interrupts();
  // Calculating the distance
  distance = duration*0.034/2;

  return distance;
}

void startWarningBatteryLow()
{
    tone(_buzzerPin, _batteryFrequency); // Send 1KHz sound signal...
}

void stopWarningBatteryLow()
{
   noTone(_buzzerPin);
}

void startWarningMalfunctioning() {
  tone(_buzzerPin, _malfunctioningFrequency);
}

void stopWarningMalfunctioning() {
  noTone(_buzzerPin);
}

void beeping(int frequency)
{
   tone(_buzzerPin, frequency);
   delay(300);
   noTone(_buzzerPin);

}




/* ------ LIBRARY HEADER FUNCTIONS ------*/
void initSEF() {
  initVibration();
  initHCSR04();
  initBuzzer();
  initSwitch();
}

unsigned int readDistance() {
  return hc_read();
}


bool obstacleDetected() {
  // Does 3 readings. Power-down mode is entered in between each of them for 30ms.
  // An obstacle is actually considered to be present of at least 2 of the 3 readings are positive
  int positives = 0;
  bool lowBattery;

  noInterrupts();
  int currentThreshold = threshold;
  interrupts();

  for (int i = 0; i < _numReadings; i++) {
    if (hc_read() <= currentThreshold) {
      positives++;
    }

    // Don't sleep on last iteration
    if (i != (_numReadings - 1)) {
      LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
    }
  }

  if (positives >= _minPositives) {
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

void userWarningVariable(int detectionResult) {
  noInterrupts();
  int currentThreshold = threshold;
  interrupts();

  // --- Implementation Option B (as described in documentation)
  if (detectionResult > currentThreshold) {
    // if not found, sleep for 280ms
    digitalWrite(_vibrationPin, LOW);
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
  } else if (detectionResult <= currentThreshold/3) {
    // Vibrate for 280ms
    digitalWrite(_vibrationPin, HIGH);
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
    digitalWrite(_vibrationPin, LOW);
  } else if (detectionResult <= 2*currentThreshold/3) {
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
    digitalWrite(_vibrationPin, LOW);
  }
}


bool testBatteryLow()
{

  bool lowBattery = false;

   float low = 3.2; //something we will have to define ourselves
   int sensorValue = analogRead(A0); //read the A0 pin value
   //the analog reading goes from 0 to 1023 and we need to convert it as a voltage from 0 to 5V
   float voltage = sensorValue * (5.00 / 1023.00); //convert the value to a true voltage.

   #ifdef DEBUG
   Serial.flush();
   Serial.println(voltage);
   Serial.flush();
   #endif

   if (voltage <= low) {
        lowBattery = true;
   }

   return lowBattery;
}


bool testSensorActive()
{
  int duration;
  bool sensorResponding = true; // Status defaults to true, later changed to false if needed
  //test if a sensor is active by trying to detect if it detects something
   // Sending a pulse for 10 µs
  digitalWrite(_hcTrigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(_hcTrigPin,LOW);

  // Reading the echo pin for the return of the pulse
  duration = pulseIn(_hcEchoPin, HIGH);

  //pulseIn() method returns the length of the pulse (in microseconds) or 0 if no pulse started before the timeout (unsigned long)
  if (duration == 0)
  {
     sensorResponding = false;
  }
  return sensorResponding;
}

bool testVibrationActive()
{
  // Momentarily set the vibrating unit input with pullup
  pinMode(_vibrationPin, INPUT_PULLUP);

  // Try to read from it: normally, the result will be low.
  // If the result is high, the pullup is functioning and the connection is floating. Therefore, the vibrating unit is disconnected
  if ( digitalRead(_vibrationPin) == HIGH )
  {
       pinMode(_vibrationPin, OUTPUT);
       digitalWrite(_vibrationPin, LOW);
       return false;
  }
  else
  {
       pinMode(_vibrationPin, OUTPUT);
       digitalWrite(_vibrationPin, LOW);
       return true;
  }
}

int getThreshold() {
  noInterrupts();
  int currentThreshold = threshold;
  interrupts();
  return currentThreshold;
}

int obstacleDistance() {
  // Does 3 readings. Power-down mode is entered in between each of them for 30ms.
  // An obstacle is actually considered to be present of at least 2 of the 3 readings are positive.
  // The readings and positives are stored in a readings and flags vector
  int positives = 0;
  int readings[_numReadings], flags[_numReadings];
  int distance = 0;
  noInterrupts();
  int currentThreshold = threshold;
  interrupts();

  for (int i = 0; i < _numReadings; i++) {
    if ((readings[i] = hc_read()) <= currentThreshold) {
      // Remember what readings were positives and which were negatives
      flags[i] = 1;
      positives++;
    } else {
      flags[i] = 0;
    }

    // Don't sleep on last iteration
    if (i != 2) {
      LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
    }
  }


  if (positives >= _minPositives) {
    // If the positives were greater or equal than the minimum, average and return them
    for (int i = 0; i < _numReadings; i++) {
      if (flags[i]) {
        distance += readings[i];
      }
    }
    distance /= positives;
    return distance;
  } else {
    // If there were fewer positives than the minimum, average the negatives and return them
    for (int i = 0; i < _numReadings; i++) {
      if (!flags[i]) {
        distance += readings[i];
      }
    }
    distance /= (_numReadings - positives);
    return distance;
  }
}

void malfunctionHalt() {
  startWarningMalfunctioning();
  delay(1000);
  stopWarningMalfunctioning();

  #ifdef DEBUG
  Serial.println("Malfunctioning error, going in permanent sleep");
  Serial.flush();
  #endif

  LowPower.powerDown(SLEEP_FOREVER,ADC_OFF,BOD_OFF);
}

void toggleMode() {
  if (mode == OUTDOOR) {
    mode = INDOOR;
  } else {
    mode = OUTDOOR;
  }
  setMode(mode);
}

void setMode(int mode){
  if (mode == OUTDOOR) { // OUTDOOR mode
    threshold = 300;
  } else { // INTDOOR mode
    threshold = 150;
  }
}
