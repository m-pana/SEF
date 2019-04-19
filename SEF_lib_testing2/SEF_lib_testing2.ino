#include <SEF.h>

bool detectionResult = false;

void setup() {
  initSEF();
  Serial.begin(9600);
}

void loop() {
  detectionResult = obstacleDetected();

  // printing to serial monitor for debugging purposes
  Serial.flush();
  String out = (detectionResult ? "TRUE" : "FALSE");
  Serial.println(out);
  Serial.flush();
  
  userWarning(detectionResult);
}
