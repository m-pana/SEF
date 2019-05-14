#include <SEF.h>
#include <LowPower.h>

int detectionResult = 0;
char out[100];

void setup() {
  initSEF();
  Serial.begin(9600);
}

void loop() {
  
  bool sensorWorks = sensorActive();
  bool vibrationWorks = true; //vibratorActive();  

  if( (sensorWorks == true) && (vibrationWorks == true) ) 
  {
    detectionResult = obstacleDistance();
    
    // printing to serial monitor for debugging purposes
    Serial.flush();
    sprintf(out, "%d, Threshold: %d, Sub-limits: %d, %d", detectionResult, getThreshold(), getThreshold()/3, 2*getThreshold()/3);
    Serial.println(out);
    Serial.flush();
  
    userWarningVariable(detectionResult);
      /* --- BOOLEAN VERSION OF DETECTION (IMPLEMENTAION A)
       * detectionResult = obstacleDetected();
    
      // printing to serial monitor for debugging purposes
      Serial.flush();
      String out = (detectionResult ? "TRUE" : "FALSE");
      Serial.println(out);
      Serial.flush();
      
      userWarning(detectionResult); */
  }
  else 
  {
      LowPower.powerDown(SLEEP_FOREVER,ADC_OFF,BOD_OFF);
  }
 


}
