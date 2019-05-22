#include <SEF.h>
#include <LowPower.h>

int detectionResult = 0;
bool detectionResultBool;
int th = getThreshold();
int countBatteryWarnings = 0;
char out[100];

void setup() {
  initSEF();
  Serial.begin(9600); // For debugging purposes
}

void loop() {
  // Battery level is tested before BDP
  if (countBatteryWarnings < 5 && testBatteryLow() == true) {
    countBatteryWarnings++;
    batteryLowWarning();
  }

  if (testSensorActive() == true && testWarningActive() == true) {
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
