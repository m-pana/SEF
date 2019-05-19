#include <SEF.h>
#include <LowPower.h>

int detectionResult = 0;
bool detectionResultBool;
int th = getThreshold();
int countBatteryWarnings = 0;
char out[100];

void setup() {
  initSEF();
  Serial.begin(9600);
  beeping(1000);
  delay(500);
  beeping(1000);
  delay(500);
}

void loop() {
  // Battery level is tested before BDP
  if (countBatteryWarnings < 5 && testBatteryLow() == true) {
    startWarningBatteryLow();
    countBatteryWarnings++;
    delay(100);
    stopWarningBatteryLow();
  }
  
  if (testSensorActive() == true && testVibrationActive() == true) {
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
