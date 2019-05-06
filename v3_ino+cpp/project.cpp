/**
  \file SEF.c
  Implementation of the SEF library.
*/

#include "Arduino.h"
#include "SEF.h"
#include "LowPower.h"

/* ------ PIN LAYOUT ------ */
#define _buzzerPin 6 
#define _vibrationPin 7
#define _hcTrigPin 8
#define _hcEchoPin 9
#define _numReadings 3
#define _minPositives 2
#define _modeSwitch 4 // TBD, switch to trigger the interrupt

unsigned int threshold = 150;
int mode; // Value for the interrupt (indoor or outdoor)

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

  // Buzzer setup 
  pinMode(_buzzerPin, OUTPUT);
}


void initInterrupt(){
  pinMode(_modeSwitch, INPUT_PULLUP); // Setting up the pullup
  mode = (digitalRead(_modeSwitch) == HIGH ? OUTDOOR : INDOOR);
  setMode(mode);

  //Setting up the ISR to trigger when a change happens in _modeSwitch pin
  attachInterrupt(digitalPinToInterrupt(INTPIN), toggleMode, CHANGE);
}

unsigned int readDistance() {
  return hc_read();
}

bool obstacleDetected() {
  // Does 3 readings. Power-down mode is entered in between each of them for 30ms.
  // An obstacle is actually considered to be present of at least 2 of the 3 readings are positive
  int positives = 0;

  for (int i = 0; i < _numReadings; i++) {
    if (hc_read() <= threshold) {
      positives++;
    }

    
    if (i==0)
    {
      //checks battery level on first reading 
      bool lowBattery = sendWarning(); 
    }

    // Don't sleep on last iteration
    if (i != (_numReadings - 1)) {
      LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF); 

      if (lowBattery == true)
      {
         //we turn off the buzzer activated by sendWarning() if the battery is low at the 3rd reading 
         stopBatterySignal();
      }
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
  // --- Implementation Option B (as described in documentation)
  if (detectionResult > threshold) {
    // if not found, sleep for 280ms
    digitalWrite(_vibrationPin, LOW);
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
  } else if (detectionResult <= threshold/3) {
    // Vibrate for 280ms
    digitalWrite(_vibrationPin, HIGH);
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
    digitalWrite(_vibrationPin, LOW);
  } else if (detectionResult <= 2*threshold/3) {
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

void startBatterySignal()
{
    tone(buzzer, 1000); // Send 1KHz sound signal...
}

void stopBatterySignal();
{
   noTone(_buzzerPin); 
}


bool sendWarning()
{ 

  bool lowBattery = false; 

   int low = 2; //something we will have to define ourselves
   int sensorValue = analogRead(A0); //read the A0 pin value
   //the analog reading goes from 0 to 1023 and we need to convert it as a voltage from 0 to 5V
   float voltage = sensorValue * (5.00 / 1023.00) * 2; //convert the value to a true voltage.

   if (voltage <= low)
   {    
        lowBattery = true; 
        startBatterySignal();
        
       
   } 

   //decided to stop the tone in other parts of the code, since the duration of the sound depends on the signal 
   return lowBattery;


}

bool sensorActive()
{
  int duration;
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
     tone(buzzer, 2000); // Send 1KHz sound signal...
     delay(1000);        // ...for 1 sec
     noTone(buzzer);   

     return false;
  }
  // DEBUG PURPOSES: return type was incorrect. returning true to make the whole thing compile. Needs to be changed.
  return true;
}

bool vibratorActive() 
{  
   pinMode(_vibrationPin, INPUT_PULLUP);     

   if ( digitalRead(_vibrationPin) == HIGH )
   {   
       pinMode(_vibrationPin, OUTPUT);
       return false;
   }
   else 
   {   
       pinMode(_vibrationPin, OUTPUT);
       return true;
   } 


}

int getThreshold() {
  return threshold;
}

int obstacleDistance() {
  // Does 3 readings. Power-down mode is entered in between each of them for 30ms.
  // An obstacle is actually considered to be present of at least 2 of the 3 readings are positive.
  // The readings and positives are stored in a readings and flags vector
  int positives = 0;
  int readings[_numReadings], flags[_numReadings];
  int distance = 0;

  for (int i = 0; i < _numReadings; i++) {
    if ((readings[i] = hc_read()) <= threshold) {
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


void toggleMode() {
  mode = !mode;
  setMode(mode);
}

void setMode(int mode){
  if(mode){ // OUTDOOR mode
    threshold=150; // 150cm, TBD
  }
  else{ // INTDOOR mode
    threshold=50; // 50cm, TBD
  }
}