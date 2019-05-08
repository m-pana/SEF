#include <SEF.h>
#include <LowPower.h>

int detectionResult = 0;
bool detectionResultBool;
int th = getThreshold();
char out[100];

void setup() {
  initSEF();
  Serial.begin(9600);
  tone(6, 440);
  delay(500);
  noTone(6);
}

void loop() {

  if ( testSensorActive() == true && testVibrationActive() == true) {
    //detectionResultBool = obstacleDetected();
    detectionResult = obstacleDistance();

    // printing to serial monitor for debugging purposes
    noInterrupts();
    Serial.flush();
    //sprintf(out, "%s, Threshold: %d, Sub-limits: %d, %d", (detectionResultBool ? "TRUE" : "FALSE"), getThreshold(), getThreshold()/3, 2*getThreshold()/3);
    sprintf(out, "%d, Threshold: %d, Sub-limits: %d, %d", detectionResult, getThreshold(), getThreshold()/3, 2*getThreshold()/3);
    Serial.println(out);
    Serial.flush();
    interrupts();

    userWarningVariable(detectionResult);
    //userWarning(detectionResultBool);
  } else {
    malfunctionHalt();
  }

}
